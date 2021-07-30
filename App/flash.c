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
	}

	if (checksum == *FLASH_CHECKSUM)
		return true;
	return false;
}

void readFlash(void)
{
	if (confirmChecksum(FLASH_D_ADDR, sizeof(flash_t) - 1)) {
		memcpy(&flash, FLASH_D_ADDR, sizeof(flash_t));
		memcpy(&g_emPhaseCalibration, &flash.calib, sizeof(EM_Phase_Calibration));
#ifdef __TEST__
		print("read flash!\n\r");
#endif
	} else {
		g_emPhaseCalibration[EM_PH_A_IDX].voltageSF = 1300572470;
		g_emPhaseCalibration[EM_PH_A_IDX].currentSF = 56519282;
		g_emPhaseCalibration[EM_PH_A_IDX].activePowerSF = 281671758;
		g_emPhaseCalibration[EM_PH_A_IDX].reactivePowerSF = 281671758;
		g_emPhaseCalibration[EM_PH_A_IDX].phaseCorrection = 0x80;

		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].voltageSF = 397821472;
		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].currentSF = 79741616;
		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].activePowerSF = 121616250;
		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].reactivePowerSF = 121616250;
		g_emPhaseCalibration[EM_PH_NEUTRAL_IDX].phaseCorrection = 0;
	}
};

void writeFlashCalib(void)
{
	uint8_t *ptr = (uint8_t *)&flash;
	uint8_t len = sizeof(flash_t);
	uint8_t checksum = calcChecksum(ptr, len - 1);

	flash.checksum = checksum;

	FlashCtl_eraseSegment(FLASH_D_ADDR);
	FlashCtl_write8(ptr, FLASH_D_ADDR, len);
}
