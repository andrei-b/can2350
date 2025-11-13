#include "xl2515.h"
#include <stdint.h>

// MCP2515 register addresses (standard 11-bit ID only)
#define MCP_RXF0SIDH 0x00
#define MCP_RXF0SIDL 0x01
#define MCP_RXM0SIDH 0x20
#define MCP_RXM0SIDL 0x21

#define MCP_RXF1SIDH 0x04
#define MCP_RXF1SIDL 0x05
#define MCP_RXM1SIDH 0x24
#define MCP_RXM1SIDL 0x25

// --- internal: pack 11-bit ID into MCP2515 SIDH/SIDL ---
static void id_to_regs(uint16_t id, uint8_t *sidh, uint8_t *sidl) {
    *sidh = (id >> 3) & 0xFF;
    *sidl = (id & 0x07) << 5;  // SID2..0 in bits 7..5
}

// --- public API ---
/**
 * Configure filter+mask pair on MCP2515/XL2515
 * @param filt_no 0..5 filter number
 * @param mask_no 0..1 mask number (0 = RXM0, 1 = RXM1)
 * @param id      CAN identifier to accept
 * @param mask    Bit mask (1=compare, 0=don't care)
 */
void xl2515_set_filter(uint8_t filt_no, uint8_t mask_no, uint16_t id, uint16_t mask) {
    uint8_t sidh, sidl;

    // Program mask
    id_to_regs(mask, &sidh, &sidl);
    if (mask_no == 0) {
        xl2515_write_reg(MCP_RXM0SIDH, sidh);
        xl2515_write_reg(MCP_RXM0SIDL, sidl);
    } else {
        xl2515_write_reg(MCP_RXM1SIDH, sidh);
        xl2515_write_reg(MCP_RXM1SIDL, sidl);
    }

    // Program filter
    id_to_regs(id, &sidh, &sidl);
    switch (filt_no) {
        case 0:
            xl2515_write_reg(MCP_RXF0SIDH, sidh);
            xl2515_write_reg(MCP_RXF0SIDL, sidl);
            break;
        case 1:
            xl2515_write_reg(MCP_RXF1SIDH, sidh);
            xl2515_write_reg(MCP_RXF1SIDL, sidl);
            break;
        // extend with RXF2..RXF5 if you need them
    }
}
