#include "flash.h"
#include <string.h>
#include "EM_userConfig.h"
#include "flashctl.h"
#include "shell.h"

extern EM_Phase_Calibration g_emPhaseCalibration[EM_PHASE_LENGTH];
flash_t flash;

uint8_t calcChecksum(uint8_t *p, uint8_t len)
{
	uint8_t checksum = 0;
	for (int i = 0; i < len; i++) {
		checksum += *p++;
	}

	return checksum;
}

bool confirmChecksum(uint8_t *p, uint8_t len)
{
	uint8_t checksum = 0;
	for (int i = 0; i < len; i++) {
		checksum += *p++;
		print("0x%02X ", *p);
	}

	if (checksum == *FLASH_CHECKSUM)
		return true;
	return false;
}

void readFlash(void)
{
	if (confirmChecksum(FLASH_D_ADDR, sizeof(flash_t) - 1)) {
		print("Read Flash Success!\n\r");

		memcpy(&flash, FLASH_D_ADDR, sizeof(flash_t));
		memcpy(&g_emPhaseCalibration, &flash.calib, sizeof(flash.calib));
	} else {
		print("Flash Checksum Fail!\n\r");

		g_emPhaseCalibration[EM_PH_A_IDX].voltageSF = 1300572470;
		g_emPhaseCalibration[EM_PH_A_IDX].currentSF = 56519282;
		g_emPhaseCalibration[EM_PH_A_IDX].activePowerSF = 281671758;
		g_emPhaseCalibration[EM_PH_A_IDX].reactivePowerSF = 281671758;
		g_emPhaseCalibration[EM_PH_A_IDX].phaseCorrection = 0x80;

		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].voltageSF = 1300572470;
		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].currentSF = 56519282;
		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].activePowerSF = 281671758;
		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].reactivePowerSF = 281671758;
		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].phaseCorrection = 0x80;

		writeFlashCalib();
	}
};

void writeFlashCalib(void)
{
	memcpy(&flash.calib, &g_emPhaseCalibration, sizeof(flash.calib));

	uint8_t *ptr = (uint8_t *)&flash;
	uint8_t len = sizeof(flash_t);
	uint8_t checksum = calcChecksum(ptr, len - 1);

	flash.checksum = checksum;

	FlashCtl_eraseSegment(FLASH_D_ADDR);
	FlashCtl_write8(ptr, FLASH_D_ADDR, len);
}
