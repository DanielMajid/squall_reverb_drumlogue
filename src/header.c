/**
 *  @file header.c
 *  @brief drumlogue SDK unit header — Squall reverb
 *
 *  Squall is a port of Mutable Instruments Clouds reverb for drumlogue.
 *  Original Clouds DSP copyright Emilie Gillet, released under MIT License.
 *  Port by Daniel Majid Mirzakhani.
 */

#include "unit.h"  // Include common definitions for all units

// ---- Unit header definition  -----------------------------------------------

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),
    .target = UNIT_TARGET_PLATFORM | k_unit_module_revfx,
    .api = UNIT_API_VERSION,
    .dev_id = 0x4D616A69U,
    .unit_id = 0x00000001U,
    .version = 0x00001000U,
    .name = "Squall",
    .num_presets = 0,
    .num_params = 4,

    .params = {
        // Page 1: the complete set of user controls. TONE and DPTH use the
        // 10-bit range so the bridge can apply smooth internal scaling.
        // Slot 0 — TONE: gentle low-pass control on reverb tail.
        {0, 1023, 0, 384, k_unit_param_type_none, 1, 0, 0, {"TONE"}},
        // Slot 1 — DPTH: reverb tail length and decay character.
        {0, 1023, 0, 512, k_unit_param_type_none, 1, 0, 0, {"DPTH"}},
        // Slot 2 — FREEZE: holds the reverb tail.
        {0, 1, 0, 0, k_unit_param_type_onoff, 0, 0, 0, {"FREEZE"}},
        // Slot 3 — SCAN: shapes the held tail while FREEZE is active.
        {0, 1023, 0, 1023, k_unit_param_type_none, 1, 0, 0, {"SCAN"}},

        // Page 2
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 3
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 4
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 5
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 6
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
    },
};
