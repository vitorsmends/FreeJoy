#include "bsp/board_api.h"
#include "tusb.h"
#include "joystick_report.h"

int main(void)
{
    board_init();
    tusb_rhport_init_t init = { .role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_FULL };
    tusb_init(0, &init);
    uint32_t previous = 0;
    while (1) {
        tud_task();
        uint32_t now = board_millis();
        board_led_write((now % (tud_mounted() ? 1000U : 250U)) < 100U);
        if ((uint32_t)(now - previous) >= 10U && tud_hid_ready()) {
            uint8_t report[JOYSTICK_REPORT_SIZE];
            joystick_report_generate(now, report);
            if (tud_hid_report(0, report, sizeof(report))) previous = now;
        }
    }
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                             hid_report_type_t type, uint8_t *buffer, uint16_t size)
{
    (void)instance;
    if (report_id != 0 || type != HID_REPORT_TYPE_INPUT) return 0;
    uint8_t report[JOYSTICK_REPORT_SIZE];
    joystick_report_generate(board_millis(), report);
    uint16_t count = size < sizeof(report) ? size : sizeof(report);
    memcpy(buffer, report, count);
    return count;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                          hid_report_type_t type, uint8_t const *buffer, uint16_t size)
{
    (void)instance; (void)report_id; (void)type; (void)buffer; (void)size;
}
