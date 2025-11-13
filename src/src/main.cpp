#include "slcan.h"
#include "pico/stdlib.h"   // stdio_init_all, gpio, etc.
#include "bsp/board_api.h" // board_init()
#include "pico/stdio/driver.h"
#include "tusb.h"          // tusb_init(), tud_task(), tud_cdc_n_*
#include <cstdio>
#include <cstring>         // memcpy()
#include "xl2515.h"        // your CAN driver

// Example CAN IDs
uint32_t id = 0x123;
uint8_t data[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdint>

// Simple parser for "t1238...." format

const uint BUF_SIZE = 32;
static uint buffer_index = 0;
char buffer[BUF_SIZE];

static void cdc0_to_can_task() {
    while ( tud_cdc_n_connected(0) && tud_cdc_n_available(0) ) {
        buffer[buffer_index] = tud_cdc_n_read_char(0);
        if (buffer[buffer_index] == '\r') {
            buffer[buffer_index] = '\0';
        }
        if (buffer[buffer_index] == '\n') {
            buffer[buffer_index] = '\0';
            buffer_index = 0; // Reset for next command
            slcan_handle_cmd(buffer);
            return;
        }
        buffer_index = (buffer_index + 1) % BUF_SIZE;
    }
}

static void can_to_cdc0_task() {
    uint32_t rx_id;
    uint8_t rx_buf[8];
    uint8_t rx_len;

    if (xl2515_recv_any(&rx_id, rx_buf, &rx_len)) {
        uint8_t out[16];
        printf("Received from CAN: ID=0x%03X LEN=%d \r\n", rx_id, rx_len);
        out[0] = rx_id & 0xFF;
        out[1] = (rx_id >> 8) & 0xFF;
        out[2] = (rx_id >> 16) & 0xFF;
        out[3] = rx_len;
        memcpy(&out[4], rx_buf, rx_len);
        if (tud_cdc_n_connected(0) && tud_cdc_n_write_available(0) >= (4 + rx_len)) {
            printf(" Sending to CDC0...\r\n");
        tud_cdc_n_write(0, out, 4 + rx_len);
        tud_cdc_n_write_flush(0);
        } else {
            printf("CDC0 not connected or not enough space to send data\r\n");
        }
    }
}

extern "C" { int _write(int, const void* buf, size_t count) {
    // Route all printf/stdout to CDC1
    if (tud_cdc_n_connected(1)) {
        tud_cdc_n_write(1, buf, count);
        tud_cdc_n_write_flush(1);

    }
    return (int)count;
}


void cdc1_out(const char *buf, int len) {
    if (!tud_cdc_n_connected(1)) return;
    tud_cdc_n_write(1, buf, len);
    tud_cdc_n_write_flush(1);
}

void cdc1_flush() {
    if (tud_cdc_n_connected(1)) {
        tud_cdc_n_write_flush(1);
    }
}

static void (*registered_callback)(void *) = NULL;
static void *registered_param = NULL;

// Register the callback
void set_chars_available_callback(void (*fn)(void *), void *param) {
    registered_callback = fn;
    registered_param = param;
}

int _in(char *buf, int len) {
    return 0;  // No input
}

void _enable_input(bool enable) {
    // Optional, but must not crash
}


stdio_driver_t cdc1_driver = {
    .out_chars = cdc1_out,
    .out_flush = cdc1_flush,
    .in_chars = _in,
    .set_chars_available_callback = set_chars_available_callback,
    .next = NULL,
    //.enable_input = _enable_input,
};
}

bool string_compare(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return false;
        a++;
        b++;
    }
    return *a == *b;
}

static void debug_task() {
    static char buf[128];
    static size_t idx = 0;

    // Process incoming chars
    while (tud_cdc_n_connected(1) && tud_cdc_n_available(1)) {
        int ch = tud_cdc_n_read_char(1);
         if (ch < 0) break;

        if (ch == '\b' || ch == 127) { // Handle backspace (ASCII 8 or 127)
            if (idx > 0) {
                idx--; // Remove last character from buffer
                tud_cdc_n_write_char(1, '\b'); // Move cursor back
                tud_cdc_n_write_char(1, ' ');  // Overwrite with space
                tud_cdc_n_write_char(1, '\b'); // Move cursor back again
                tud_cdc_n_write_flush(1);
            }
        } else
        { 
            tud_cdc_n_write_char(1, ch);
            tud_cdc_n_write_flush(1);
            
            if (ch == '\r' || ch == '\n') {
            // End of line
            buf[idx] = '\0';
            if (idx > 0) {
                printf("\nCDC1 received line: %s\r\n", buf);
                idx = 0;
                if (string_compare(buf, "hello")) {
                    printf("Hello to you too!\r\n");
                }
            }
        } else if (idx < sizeof(buf) - 1) {
            // Store character
            buf[idx++] = (char)ch;
        }
        }
    }
}



int main() {
    board_init();
    stdio_init_all();
    stdio_set_driver_enabled(&stdio_usb, false);
    stdio_set_driver_enabled(&cdc1_driver, true);
    tusb_init();

    slcan_init(125000); // Default 125 kbps, closed until "O" command
    slcan_open_can();
    printf("Starting up...\r\n");


    while (true) {
        tud_task();           // USB
        //can_to_cdc0_task();   // CAN → USB
        //cdc0_to_can_task();   // USB → CAN  
        cdc0_to_can_task(); // SLCAN USB → CAN
        slcan_can_to_usb(); // SLCAN CAN → USB
        debug_task();         // Debug on CDC1
        tight_loop_contents();
        //printf(".");
    }
}
