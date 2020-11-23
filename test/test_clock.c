#include "shell.h"
#include "ucs.h"

void test_clock(void) {
  uint32_t mclk  = UCS_getMCLK();
  uint32_t smclk = UCS_getSMCLK();
  uint32_t aclk  = UCS_getACLK();
  print("mclk clock = %ld\n\r", mclk);
  print("smclk clock = %ld\n\r", smclk);
  print("aclk clock = %ld\n\r", aclk);
}
