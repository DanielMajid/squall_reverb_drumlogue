/*
 *  File: clouds_reverb.cc
 *
 *  drumlogue reverb bridge for the Clouds reverb core.
 *
 *  This file owns runtime hook wiring, parameter forwarding, and the static
 *  delay memory used in place of sdram_alloc. DSP behavior is implemented in
 *  reverb.h.
 *
 *  Clouds DSP Copyright (c) 2014 Emilie Gillet, released under MIT License.
 *  Port by Daniel Majid Mirzakhani.
 */

#include <algorithm>
#include <cstdint>

#include "reverb.h"
#include "unit.h"

namespace {

// Reverb working memory in float words.
// The capacity covers every delay line and filter state used by the core.
constexpr uint32_t k_reverb_buffer_words = 16384U;

// Parameter indices consumed by _hook_param.
constexpr uint8_t k_param_tone = 0U;
constexpr uint8_t k_param_depth = 1U;
constexpr uint8_t k_param_mix_passthrough = 2U;
constexpr uint8_t k_param_freeze = 3U;
constexpr uint8_t k_param_freeze_scan = 4U;

// Reverb core scaling constants define Squall's input level and decay range.
constexpr float k_amount_scale = 0.54f;
constexpr float k_time_base = 0.35f;
constexpr float k_time_scale = 0.63f;
constexpr float k_lp_base = 0.6f;
constexpr float k_lp_scale = 0.37f;
constexpr float k_input_gain_base = 0.2f;

// Per-sample coefficients match the Clouds core's 32-frame control response
// while supporting the callback sizes used by the drumlogue SDK.
constexpr float k_control_smoothing = 0.00694898f;
constexpr float k_freeze_smoothing = 0.00127487f;
constexpr float k_scan_smoothing = 0.00047219f;
constexpr float k_freeze_input_bleed_max = 0.05f;
constexpr float k_freeze_leak_out_when_scan_open = 0.005f;
constexpr float k_scan_lp_sweep = 0.24f;
constexpr float k_scan_diffusion_base = 0.50f;
constexpr float k_scan_diffusion_sweep = 0.22f;

// Static delay buffer provides working memory without runtime allocation.
static float s_reverb_buffer_storage[k_reverb_buffer_words];

static clouds::Reverb241A2A9 s_processor_instance;
static float * s_reverb_buffer = nullptr;

static float s_reverb_amount = 0.f;
static float s_reverb_amount_smoothed = 0.f;
static float s_tone = 1.f;
static float s_tone_smoothed = 1.f;
static float s_freeze_scan = 0.f;
static bool s_freeze = false;
static float s_freeze_blend = 0.f;
static float s_scan_smoothed = 0.f;

inline float clamp01(const float x) {
  return x < 0.f ? 0.f : (x > 1.f ? 1.f : x);
}

inline void smooth_controls(uint32_t frames) {
  for (uint32_t i = 0U; i < frames; ++i) {
    ONE_POLE(s_reverb_amount_smoothed, s_reverb_amount, k_control_smoothing);
    ONE_POLE(s_tone_smoothed, s_tone, k_control_smoothing);
    ONE_POLE(s_freeze_blend, s_freeze ? 1.f : 0.f, k_freeze_smoothing);
    ONE_POLE(s_scan_smoothed, s_freeze_scan, k_scan_smoothing);
  }
}

}  // namespace

extern "C" {

// Called by unit.cc before _hook_init to bind the static buffer.
void _hook_set_static_buffer(void) {
  s_reverb_buffer = s_reverb_buffer_storage;
  std::fill_n(s_reverb_buffer, k_reverb_buffer_words, 0.f);
}

void _hook_init(uint32_t platform, uint32_t api, uint32_t samplerate) {
  (void)platform;
  (void)api;

  s_processor_instance.Init(s_reverb_buffer, static_cast<float>(samplerate));
  s_reverb_amount = 0.f;
  s_reverb_amount_smoothed = 0.f;
  s_tone = 1.f;
  s_tone_smoothed = 1.f;
  s_freeze_scan = 0.f;
  s_freeze = false;
  s_freeze_blend = 0.f;
  s_scan_smoothed = 0.f;
}

void _hook_resume(void) {}

void _hook_suspend(void) {}

void _hook_process(float * xn, uint32_t frames) {
  smooth_controls(frames);

  const float base_amount = s_reverb_amount_smoothed * k_amount_scale;
  const float base_time = k_time_base + k_time_scale * s_reverb_amount_smoothed;
  const float base_lp = k_lp_base + k_lp_scale * s_tone_smoothed;

  const float scan_lock = clamp01((s_scan_smoothed - 0.5f) * 2.f);
  const float scan_leak = clamp01((0.5f - s_scan_smoothed) * 2.f);

  const float input_bleed = scan_leak * k_freeze_input_bleed_max;
  const float freeze_leak_out = scan_leak > 0.f ? k_freeze_leak_out_when_scan_open : 0.f;
  const float freeze_hold = s_freeze_blend * (1.f - freeze_leak_out);
  const float input_scale = (1.f - s_freeze_blend) + s_freeze_blend * input_bleed;

  const float scan_color = scan_lock * scan_lock * (0.75f + 0.25f * scan_lock);
  const float scan_amount = scan_color * s_freeze_blend;

  const float amount = base_amount + (1.f - base_amount) * freeze_hold;
  const float reverb_time = base_time + (1.f - base_time) * freeze_hold;
  const float freeze_wet_gain = 1.f - 0.45f * s_freeze_blend;
  const float input_gain = k_input_gain_base * input_scale;

  float lp = base_lp + (1.f - base_lp) * freeze_hold;
  lp += k_scan_lp_sweep * scan_amount;
  lp = clamp01(lp);

  float diffusion = k_scan_diffusion_base + k_scan_diffusion_sweep * scan_amount;
  if (diffusion > 0.74f)
    diffusion = 0.74f;

  s_processor_instance.set_amount(amount);
  s_processor_instance.set_diffusion(clamp01(diffusion));
  s_processor_instance.set_time(reverb_time);
  s_processor_instance.set_input_gain(input_gain);
  s_processor_instance.set_lp(lp);
  s_processor_instance.set_freeze_wet_gain(freeze_wet_gain);

  clouds::FloatFrame * out = reinterpret_cast<clouds::FloatFrame *>(xn);
  s_processor_instance.Process(out, frames);
}

void _hook_param(uint8_t index, int32_t value) {
  const float value_f = static_cast<float>(value) * (1.f / 1023.f);

  switch (index) {
    case k_param_tone:
      s_tone = clamp01(value_f);
      break;
    case k_param_depth:
      s_reverb_amount = clamp01(value_f);
      break;
    case k_param_mix_passthrough:
      // Intentionally inert to preserve Squall's Clouds-style playability.
      break;
    case k_param_freeze:
      s_freeze = value != 0;
      break;
    case k_param_freeze_scan:
      s_freeze_scan = clamp01(value_f);
      break;
    default:
      break;
  }
}

}  // extern "C"
