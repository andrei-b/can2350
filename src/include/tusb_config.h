#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

/* Let Pico SDK board support select MCU/RTOS. Do NOT force CFG_TUSB_MCU here on RP2350. */

/* Device stack: enable what you actually use */
#define CFG_TUD_CDC            2    // 2 CDC ACM ports
#define CFG_TUD_VENDOR         1    // 1 vendor interface (optional)
#define CFG_TUD_HID            1    // 1 HID (optional)

/* USB device port config */
#define CFG_TUSB_RHPORT0_MODE  OPT_MODE_DEVICE
#define CFG_TUD_ENDPOINT0_SIZE 64

/* CDC buffer sizes */
#ifndef CFG_TUD_CDC_RX_BUFSIZE
#define CFG_TUD_CDC_RX_BUFSIZE 256
#endif
#ifndef CFG_TUD_CDC_TX_BUFSIZE
#define CFG_TUD_CDC_TX_BUFSIZE 256
#endif
#ifndef CFG_TUD_CDC_EP_BUFSIZE
#define CFG_TUD_CDC_EP_BUFSIZE 64
#endif

/* HID report size (generic 8 bytes in/out) */
#ifndef CFG_TUD_HID_EP_BUFSIZE
#define CFG_TUD_HID_EP_BUFSIZE 16
#endif

#endif /* _TUSB_CONFIG_H_ */
