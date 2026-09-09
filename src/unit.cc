/*
 *  File: unit.cc
 *
 *  @brief drumlogue reverb unit interface — Squall (Clouds reverb port).
 *
 *  Squall is a port of Mutable Instruments Clouds reverb for drumlogue.
 *  Original Clouds DSP copyright Emilie Gillet, released under MIT License.
 *  Port by Daniel Majid Mirzakhani.
 */

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "unit.h"

extern "C" {

void _hook_init(uint32_t platform, uint32_t api, uint32_t samplerate);
void _hook_process(float * in_out, uint32_t frames);
void _hook_suspend(void);
void _hook_resume(void);
void _hook_param(uint8_t index, int32_t value);
void _hook_set_static_buffer(void);

}  // extern "C"

enum {
  k_param_tone = 0,
  k_param_depth,
  k_param_mix,
  k_param_freeze,
  k_param_freeze_scan,
  k_num_params,
};

class Squall {
 public:
  Squall(void) {}
  ~Squall(void) {}

  inline int8_t Init(const unit_runtime_desc_t * desc) {
    if (desc->samplerate != 48000U)
      return k_unit_err_samplerate;

    if (desc->input_channels != 2U || desc->output_channels != 2U)
      return k_unit_err_geometry;

    _hook_set_static_buffer();
    _hook_init(desc->target, desc->api, desc->samplerate);

    for (uint8_t i = 0U; i < k_num_params; ++i) {
      cached_values_[i] = static_cast<int32_t>(unit_header.params[i].init);
      _hook_param(i, cached_values_[i]);
    }

    return k_unit_err_none;
  }

  inline void Teardown() {}

  inline void Reset() {
    _hook_init(0U, 0U, 48000U);
    for (uint8_t i = 0U; i < k_num_params; ++i)
      _hook_param(i, cached_values_[i]);
  }

  inline void Resume() {
    _hook_resume();
  }

  inline void Suspend() {
    _hook_suspend();
  }

  fast_inline void Process(const float * in, float * out, size_t frames) {
    if (in != out)
      std::copy(in, in + (frames << 1), out);
    _hook_process(out, static_cast<uint32_t>(frames));
  }

  inline void setParameter(uint8_t index, int32_t value) {
    if (index >= k_num_params)
      return;

    const int32_t minimum = unit_header.params[index].min;
    const int32_t maximum = unit_header.params[index].max;
    if (value < minimum)
      value = minimum;
    else if (value > maximum)
      value = maximum;

    cached_values_[index] = value;
    _hook_param(index, value);
  }

  inline int32_t getParameterValue(uint8_t index) const {
    return index < k_num_params ? cached_values_[index] : 0;
  }

  inline const char * getParameterStrValue(uint8_t index, int32_t value) const {
    (void)index;
    (void)value;
    return nullptr;
  }

  inline const uint8_t * getParameterBmpValue(uint8_t index, int32_t value) const {
    (void)index;
    (void)value;
    return nullptr;
  }

  inline void LoadPreset(uint8_t idx) {
    (void)idx;
  }

  inline uint8_t getPresetIndex() const {
    return 0U;
  }

  static inline const char * getPresetName(uint8_t idx) {
    (void)idx;
    return nullptr;
  }

 private:
  int32_t cached_values_[UNIT_MAX_PARAM_COUNT] = {};
};

static Squall s_reverb_instance;
static unit_runtime_desc_t s_runtime_desc;

// ---- Callback entry points from drumlogue runtime ----------------------------------------------

__unit_callback int8_t unit_init(const unit_runtime_desc_t * desc) {
  if (!desc)
    return k_unit_err_undef;

  if (desc->target != unit_header.target)
    return k_unit_err_target;
  if (!UNIT_API_IS_COMPAT(desc->api))
    return k_unit_err_api_version;

  s_runtime_desc = *desc;

  return s_reverb_instance.Init(desc);
}

__unit_callback void unit_teardown() {
  s_reverb_instance.Teardown();
}

__unit_callback void unit_reset() {
  s_reverb_instance.Reset();
}

__unit_callback void unit_resume() {
  s_reverb_instance.Resume();
}

__unit_callback void unit_suspend() {
  s_reverb_instance.Suspend();
}

__unit_callback void unit_render(const float * in, float * out, uint32_t frames) {
  s_reverb_instance.Process(in, out, frames);
}

__unit_callback void unit_set_param_value(uint8_t id, int32_t value) {
  s_reverb_instance.setParameter(id, value);
}

__unit_callback int32_t unit_get_param_value(uint8_t id) {
  return s_reverb_instance.getParameterValue(id);
}

__unit_callback const char * unit_get_param_str_value(uint8_t id, int32_t value) {
  return s_reverb_instance.getParameterStrValue(id, value);
}

__unit_callback const uint8_t * unit_get_param_bmp_value(uint8_t id, int32_t value) {
  return s_reverb_instance.getParameterBmpValue(id, value);
}

__unit_callback void unit_set_tempo(uint32_t tempo) {
  (void)tempo;
}

__unit_callback void unit_load_preset(uint8_t idx) {
  s_reverb_instance.LoadPreset(idx);
}

__unit_callback uint8_t unit_get_preset_index() {
  return s_reverb_instance.getPresetIndex();
}

__unit_callback const char * unit_get_preset_name(uint8_t idx) {
  return Squall::getPresetName(idx);
}
