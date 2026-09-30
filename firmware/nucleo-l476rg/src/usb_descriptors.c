#include "bsp/board_api.h"
#include "tusb.h"

/* TinyUSB example VID/PID, for local testing only; not a FreeJoy identity. */
static tusb_desc_device_t const device = {
    .bLength = sizeof(tusb_desc_device_t), .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200, .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0xCAFE, .idProduct = 0x4004, .bcdDevice = 0x0100,
    .iManufacturer = 1, .iProduct = 2, .iSerialNumber = 3,
    .bNumConfigurations = 1
};

static uint8_t const hid_report[] = {
    0x05, 0x01,       /* Generic Desktop */
    0x09, 0x04,       /* Joystick */
    0xA1, 0x01,       /* Application collection */
    0x05, 0x09,       /* Buttons */
    0x19, 0x01, 0x29, 0x10,
    0x15, 0x00, 0x25, 0x01,
    0x75, 0x01, 0x95, 0x10,
    0x81, 0x02,       /* 16 one-bit buttons */
    0x05, 0x01,
    0x09, 0x30, 0x09, 0x31, 0x09, 0x32, /* X, Y, Z */
    0x09, 0x33, 0x09, 0x34, 0x09, 0x35, /* Rx, Ry, Rz */
    0x09, 0x36, 0x09, 0x37,             /* Slider, Dial */
    0x16, 0x01, 0x80, /* Logical minimum -32767 */
    0x26, 0xFF, 0x7F, /* Logical maximum 32767 */
    0x75, 0x10, 0x95, 0x08,
    0x81, 0x02,       /* Eight absolute signed 16-bit axes */
    0xC0
};

static uint8_t const configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN,
                          TUSB_DESC_CONFIG_ATT_SELF_POWERED, 0),
    TUD_HID_DESCRIPTOR(0, 0, HID_ITF_PROTOCOL_NONE, sizeof(hid_report), 0x81, 64, 10)
};

uint8_t const *tud_descriptor_device_cb(void) { return (uint8_t const *)&device; }
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    return hid_report;
}
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index;
    return configuration;
}
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    static uint16_t result[33];
    static char const *const strings[] = { "", "FreeJoy Lab", "Nucleo Mock Joystick" };
    size_t count;
    if (index == 0) {
        result[1] = 0x0409;
        count = 1;
    } else if (index == 3) {
        count = board_usb_get_serial(result + 1, 32);
    } else if (index < sizeof(strings) / sizeof(strings[0])) {
        count = strlen(strings[index]);
        if (count > 32) count = 32;
        for (size_t i = 0; i < count; ++i) result[i + 1] = (uint8_t)strings[index][i];
    } else return NULL;
    result[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * count + 2));
    return result;
}
