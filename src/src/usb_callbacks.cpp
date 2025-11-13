#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "bsp/board.h"
#include "tusb.h"          // <-- required for TinyUSB + HID callbacks


// HID GET_REPORT callback
extern "C"
uint16_t tud_hid_get_report_cb(uint8_t instance,
                               uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t* buffer,
                               uint16_t reqlen) {
    // For now return nothing
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

// HID SET_REPORT callback
extern "C"
void tud_hid_set_report_cb(uint8_t instance,
                           uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer,
                           uint16_t bufsize) {
    // Ignore for now (no output reports)
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}
