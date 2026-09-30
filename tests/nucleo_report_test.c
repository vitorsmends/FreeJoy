#include <assert.h>
#include <stdint.h>
#include "joystick_report.h"

int main(void)
{
    uint8_t guarded[JOYSTICK_REPORT_SIZE + 2];
    memset(guarded, 0xA5, sizeof(guarded));
    for (uint32_t t = 0; t < 16000; ++t) {
        joystick_report_generate(t, guarded + 1);
        assert(guarded[0] == 0xA5 && guarded[sizeof(guarded) - 1] == 0xA5);
        uint16_t buttons = guarded[1] | ((uint16_t)guarded[2] << 8);
        assert(buttons == (t % 1000 < 500 ? (1U << (t / 1000)) : 0));
    }
    joystick_report_generate(0, guarded + 1);
    assert(guarded[3] == 0x01 && guarded[4] == 0x80);
    joystick_report_generate(2000, guarded + 1);
    assert(guarded[3] == 0 && guarded[4] == 0);
    joystick_report_generate(4000, guarded + 1);
    assert(guarded[3] == 0xFF && guarded[4] == 0x7F);
    joystick_report_generate(UINT32_MAX, guarded + 1);
    assert(guarded[0] == 0xA5 && guarded[sizeof(guarded) - 1] == 0xA5);
    return 0;
}
