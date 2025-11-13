#include "tusb.h"
#include "xl2515.h"
#include <cstdio>
#include <cctype>
#include <cstring>

// SLCAN state
static bool can_open = false;
static uint32_t can_bitrate = 500000;

// USB RX buffer
static char cmd_buf[64];
static int cmd_idx = 0;

// --- helper: map Sx -> bitrate ---
static uint32_t slcan_bitrate(char code) {
    switch (code) {
        case '0': return 10000;
        case '1': return 20000;
        case '2': return 50000;
        case '3': return 100000;
        case '4': return 125000;
        case '5': return 250000;
        case '6': return 500000;
        case '7': return 800000;
        case '8': return 1000000;
        default:  return 125000;
    }
}

// --- handle complete command ---
void slcan_handle_cmd(const char *cmd) {
    printf("Received SLCAN command: %s\r\n", cmd);
    // Set bitrate
    if (cmd[0] == 'S') {
        can_bitrate = slcan_bitrate(cmd[1]);
        // (re)init controller
        xl2515_init( (xl2515_rate_kbps_t)  can_bitrate);
        tud_cdc_n_write_str(0, "\r");
        tud_cdc_n_write_flush(0);
    }
    // Open channel
    else if (cmd[0] == 'O') {
        can_open = true;
        tud_cdc_n_write_str(0, "\r");
        tud_cdc_n_write_flush(0);
    }
    // Close channel
    else if (cmd[0] == 'C') {
        can_open = false;
        tud_cdc_n_write_str(0, "\r");
        tud_cdc_n_write_flush(0);
    }
    // Transmit standard frame
    else if (cmd[0] == 't' && can_open) {
        uint32_t id;
        int dlc;
        uint8_t data[8];
        if (sscanf(cmd + 1, "%3x%1d", &id, &dlc) == 2) {
            const char *pdata = cmd + 5;
            for (int i = 0; i < dlc && i < 8; i++) {
                unsigned int b;
                sscanf(pdata + i*2, "%2x", &b);
                data[i] = (uint8_t)b;
            }
            printf("Sending standard frame: ID=0x%03X DLC=%d\r\n", id, dlc);
            xl2515_send(id, data, dlc);
        }
    }
    // Transmit extended frame
    else if (cmd[0] == 'T' && can_open) {
        uint32_t id;
        int dlc;
        uint8_t data[8];
        if (sscanf(cmd + 1, "%3x%1d", &id, &dlc) == 2) {
            const char *pdata = cmd + 10;
            for (int i = 0; i < dlc && i < 8; i++) {
                unsigned int b;
                sscanf(pdata + i*2, "%2x", &b);
                data[i] = (uint8_t)b;
            }
            xl2515_send(id, data, dlc);
        }
    }
    // Version
    else if (cmd[0] == 'V') {
        tud_cdc_n_write_str(0, "V1013\r"); // version 1.13 example
        tud_cdc_n_write_flush(0);
    }
    // Status flags
    else if (cmd[0] == 'F') {
        tud_cdc_n_write_str(0, "F00\r");
        tud_cdc_n_write_flush(0);
    }
}

// --- USB CDC0 receive handler ---
void slcan_usb_to_can() {
    while (tud_cdc_n_available(0)) {
        printf("tud_cdc_n_available\r\n");
        char c = tud_cdc_n_read_char(0);
        printf("Received char: %c\r\n", c);
        if (c == '\r' || c == '\n') {
            cmd_buf[cmd_idx] = 0;
            if (cmd_idx > 0) {
                printf("Complete command: %s\r\n", cmd_buf);
                slcan_handle_cmd(cmd_buf);
            }
            cmd_idx = 0;
        } else {
            if (cmd_idx < (int)sizeof(cmd_buf)-1) {
                cmd_buf[cmd_idx++] = c;
            }
        }
    }
}

// --- CAN RX -> SLCAN ASCII ---
void slcan_can_to_usb() {
    if (!can_open) return;

    uint32_t id;
    uint8_t data[8];
    uint8_t dlc;

    while (xl2515_recv_any(&id, data, &dlc)) {
        char out[64];
        int len = sprintf(out, "t%03X%d", id & 0x7FF, dlc);
        for (int i = 0; i < dlc; i++) {
            len += sprintf(out+len, "%02X", data[i]);
        }
        out[len++] = '\r';
        tud_cdc_n_write(0, out, len);
        tud_cdc_n_write_flush(0);
    }
}

void slcan_init(uint32_t default_bitrate) {
    xl2515_rate_kbps_t rate = KBPS125;
    can_bitrate = default_bitrate;
    switch (default_bitrate) {
        case 5000:   rate = KBPS5; break;
        case 10000:   rate = KBPS10; break;
        case 20000:   rate = KBPS20; break;
        case 50000:  rate = KBPS50; break;
        case 125000:  rate = KBPS125; break;
        case 250000:  rate = KBPS250; break;
        case 500000:  rate = KBPS500; break;
        case 800000:  rate = KBPS800; break;
        case 1000000: rate = KBPS1000; break;
        default:      rate = KBPS125; break;
    }
    can_open = false;
    xl2515_init(KBPS125);
}

bool slcan_is_open(void) {
    return can_open;
}

uint32_t slcan_get_bitrate(void) {
    return can_bitrate;
}


// --- Public task ---
void slcan_task() {
    slcan_usb_to_can();
    slcan_can_to_usb();
}

void slcan_open_can() {
    can_open = true;
}
