#ifndef __METERING_H__
#define __METERING_H__

#include <stdint.h>
#include "emSwLib.h"

//**************************************
// Electric Meter Message
//**************************************

/* Terminal -> Meter */
#define TERMINAL_STX 0x24
#define TERMINAL_ETX 0x16

#define REQ_READ_METER 0xA0
#define SET_METER_CONFIG 0xA1
#define SET_LIGHT_TIME 0xA2
#define SET_RTC_TIME 0xA3
#define SET_CALIB_FACTOR 0xA4

typedef struct {
	uint8_t stx;
	uint8_t msgType;
	uint8_t checkSum;
	uint8_t etx;
} elecMeterReq_t;

/* Meter -> Terminal */
#define METER_STX 0x5B
#define METER_ETX 0x8D

#define RESP_READ_METER 0xB0
#define RESP_METER_CONFIG 0xB1
#define RESP_LIGHT_TIME 0xB2
#define RESP_RTC_TIME 0xB3
#define RESP_CALIB_FACTOR 0xB4

//**************************************
// Electric Meter Comm Data
//**************************************
#define LEN_MAX_METER_RX 0x40
#define LEN_MAX_METER_TX 0x60

typedef struct {
	uint8_t stx;
	uint8_t msgType;
	uint32_t VRMS;
	uint32_t IRMS;
	uint32_t shortIRMS;
	int32_t power;
	int32_t cumPower;
	uint8_t checkSum;
	uint8_t etx;
} elecMeterResp_t;

struct receive {
	char buf[LEN_MAX_METER_RX];
	char status;
	int index;
};

extern struct receive meterRx;

void req_read_meter(void);
void resp_read_meter(void);
void checkMessage(void);

#endif
