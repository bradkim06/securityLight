#include "shell.h"
#include "ucs.h"

unsigned long aclk = 0;

unsigned long test_aclk(void) {
  aclk = UCS_getACLK();
  print("aclk clock = %ld", aclk);
  return aclk;
}
