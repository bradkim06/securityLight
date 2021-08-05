#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdbool.h>
#include <stdint.h>

#include "EM_userConfig.h"

#define FLASH_VOLTAGE (uint32_t *)0x1800
#define FLASH_CURRENT (uint32_t *)0x1804
#define FLASH_ACTIVE_POWER (uint32_t *)0x1808
#define FLASH_REACTIVE_POWER (uint32_t *)0x180C
#define FLASH_PHASE_CORRECTION (uint16_t *)0x1810
#define FLASH_CHECKSUM (uint8_t *)0x1812
#define FLASH_D_ADDR (uint8_t *)0x1800

typedef struct {
	EM_Phase_Calibration calib[EM_PHASE_LENGTH];
	EM_Energy_t cumPower;
	uint8_t checksum;
} flash_t;

extern flash_t flash;

uint8_t calcChecksum(uint8_t *p, uint8_t len);
bool confirmChecksum(uint8_t *p, uint8_t len);
void readFlash(void);
void writeFlash(void);

#endif
