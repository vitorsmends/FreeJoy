#include <assert.h>
#include "tusb.h"
#include "joystick_report.h"

size_t board_get_unique_id(uint8_t *id, size_t max_len)
{
    assert(max_len >= 4);
    memset(id, 0x12, 4);
    return 4;
}

int main(void)
{
    const uint8_t *config = tud_descriptor_configuration_cb(0);
    assert(config[2] == TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN);
    assert(config[3] == 0 && config[4] == 1);
    /* Configuration, interface (9), HID (9), endpoint (7). */
    assert(config[9 + 5] == TUSB_CLASS_HID);
    assert(config[27 + 2] == 0x81);
    assert(config[27 + 6] == 10);
    unsigned length = config[18 + 7] | ((unsigned)config[18 + 8] << 8);
    const uint8_t *descriptor = tud_hid_descriptor_report_cb(0);
    assert(descriptor[0] == 0x05 && descriptor[1] == 0x01);
    assert(descriptor[2] == 0x09 && descriptor[3] == 0x04);
    unsigned bits = 0, size = 0, count = 0, collections = 0;
    for (unsigned pos = 0; pos < length;) {
        unsigned tag = descriptor[pos++];
        unsigned bytes = tag & 3U;
        if (bytes == 3) bytes = 4;
        assert(pos + bytes <= length);
        unsigned value = 0;
        for (unsigned i = 0; i < bytes; ++i) value |= (unsigned)descriptor[pos + i] << (8 * i);
        if (tag == 0x75) size = value;
        if (tag == 0x95) count = value;
        if (tag == 0x81) bits += size * count;
        if (tag == 0xA1) ++collections;
        if (tag == 0xC0) { assert(collections > 0); --collections; }
        assert(tag != 0x85); /* No report ID */
        pos += bytes;
    }
    assert(collections == 0 && bits == JOYSTICK_REPORT_SIZE * 8);
    const tusb_desc_device_t *device = (const tusb_desc_device_t *)tud_descriptor_device_cb();
    assert(device->bNumConfigurations == 1);
    assert(tud_descriptor_string_cb(2, 0x0409) != NULL);
    assert(tud_descriptor_string_cb(255, 0x0409) == NULL);
    return 0;
}
