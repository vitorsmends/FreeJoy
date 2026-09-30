#ifndef NUCLEO_JOYSTICK_REPORT_H
#define NUCLEO_JOYSTICK_REPORT_H
#include "mock_inputs.h"
#define JOYSTICK_REPORT_SIZE 18
/* Wire format: 16 button bits, then eight signed 16-bit little-endian axes. */
static inline void joystick_report_generate(uint32_t millis, uint8_t report[JOYSTICK_REPORT_SIZE])
{
    int16_t axes[MAX_AXIS_NUM];
    uint8_t buttons[MAX_BUTTONS_NUM / 8];
    MockInputsGenerate(millis, axes, buttons);
    report[0] = buttons[0];
    report[1] = buttons[1];
    for (unsigned i = 0; i < MAX_AXIS_NUM; ++i) {
        uint16_t value = (uint16_t)axes[i];
        report[2 + i * 2] = (uint8_t)value;
        report[3 + i * 2] = (uint8_t)(value >> 8);
    }
}
#endif
