#ifndef SLCAN_H
#define SLCAN_H

#include <stdint.h>
#include <stdbool.h>



// -----------------------------------------------------------------------------
// Public API for SLCAN bridge (CDC0 <-> CAN)
// -----------------------------------------------------------------------------

/**
 * @brief Initialize SLCAN bridge
 *
 * Call this once after TinyUSB and your CAN driver (xl2515_init).
 * Sets default bitrate and closes CAN channel until "O" is received.
 *
 * @param default_bitrate CAN bitrate in bits per second (e.g. 125000)
 */
void slcan_init(uint32_t default_bitrate);

/**
 * @brief Periodic task for SLCAN
 *
 * Call this often from main loop.
 * - Reads ASCII commands from CDC0 and applies them (S, O, C, t, T, …)
 * - Forwards received CAN frames as ASCII over CDC0
 */
void slcan_task(void);

/**
 * @brief Check if CAN channel is currently open (after "O" command)
 *
 * @return true if channel is open, false if closed
 */
bool slcan_is_open(void);

/**
 * @brief Get the current CAN bitrate
 *
 * This is updated whenever an "Sx" command is received.
 *
 * @return bitrate in bits per second (e.g. 125000)
 */
uint32_t slcan_get_bitrate(void);


void slcan_open_can();

void slcan_can_to_usb();
void slcan_usb_to_can();
void slcan_handle_cmd(const char *cmd);
#endif // SLCAN_H
