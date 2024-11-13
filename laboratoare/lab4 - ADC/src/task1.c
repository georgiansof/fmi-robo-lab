#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "usart.h"
#include "adc.h"
#include "task1.h"

void task1(void) {
  while (1) {
    uint16_t adcValue = myAnalogRead(0);
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "ADC Value: %d\n", adcValue);
    USART0_print(buffer);
    USART0_print("\n");
  }
}