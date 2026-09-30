#ifndef FREEJOY_MOCK_INPUTS_H
#define FREEJOY_MOCK_INPUTS_H

#include <stdint.h>
#include <string.h>
#include "common_defines.h"

/* Output-level fixture: eight 8-second triangle waves, staggered by 500 ms.
 * One of the first 16 buttons is held for 500 ms, then released for 500 ms.
 * No hardware dependencies, floating point, or mutable state.
 */
static void MockInputsGenerate(uint32_t millis, int16_t *axes, uint8_t *buttons)
{
    uint8_t i;
    for (i = 0; i < MAX_AXIS_NUM; i++)
    {
        uint32_t phase = (millis % 8000U + i * 500U) % 8000U;
        uint32_t ramp = phase <= 4000U ? phase : 8000U - phase;
        axes[i] = (int16_t)(AXIS_MIN_VALUE +
            (int32_t)(ramp * (AXIS_MAX_VALUE - AXIS_MIN_VALUE) / 4000U));
    }
    memset(buttons, 0, MAX_BUTTONS_NUM / 8);
    if (millis % 1000U < 500U)
    {
        uint8_t button = (millis / 1000U) % 16U;
        buttons[button / 8] = (uint8_t)(1U << (button % 8));
    }
}

#endif
