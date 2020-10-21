#include "gpio.h"

void test_led(void) {
  // EVM Board LED1 Pin Toggle
  GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN6);
  __delay_cycles(6400000);
}
