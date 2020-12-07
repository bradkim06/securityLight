#include "flash.h"
#include "EM_userConfig.h"
#include "flashctl.h"
#include "shell.h"

flash_t flash;

uint8_t calcChecksum(uint8_t* p, uint8_t len) {
  uint8_t checksum = 0;
  for (int i = 0; i < len; i++) {
    checksum += *p++;
  }

  return checksum;
}

bool confirmChecksum(uint8_t* p, uint8_t len) {
  uint8_t checksum = 0;
  for (int i = 0; i < len; i++) {
    checksum += *p++;
  }

  if (checksum == *FLASH_CHECKSUM) return true;
  return false;
}

void writeFlashCalib(void) {
  uint8_t* ptr      = (uint8_t*)&flash;
  uint8_t  len      = sizeof(flash_t);
  uint8_t  checksum = calcChecksum(ptr, len - 1);

  flash.checksum = checksum;

  FlashCtl_eraseSegment(FLASH_D_ADDR);
  FlashCtl_write8(ptr, FLASH_D_ADDR, len);
  // FlashCtl_write8(&checksum, FLASH_CHECKSUM, 1);

#ifdef __TEST__
  flash_t* pTest = (flash_t*)0x1800;

  print("flash length = %u\n\r", sizeof(flash_t));
  print("checksum = %u\n\r", checksum);
  print("flash_check = %u\n\r", pTest->checksum);
  print("voltage = %lu\n\r", pTest->calib.voltageSF);
  print("current = %lu\n\r", pTest->calib.currentSF);
  print("active = %lu\n\r", pTest->calib.activePowerSF);
  print("reactive = %lu\n\r", pTest->calib.reactivePowerSF);
  print("phase = %d\n\r", pTest->calib.phaseCorrection);
#endif
}
