#ifndef _JOY_TELEMETRY_CONSTANTS_H
#define _JOY_TELEMETRY_CONSTANTS_H

#define tele_DATA_COUNT  ((int) 14)
#define USB_ESCAPE_BYTE  ((uint8_t) 0xFE)
#define USB_HEADER_BYTE  ((uint8_t) 0x88)
#define tele_MAX_BUF     ( (int) (tele_DATA_COUNT*4 + 2) )

#endif

