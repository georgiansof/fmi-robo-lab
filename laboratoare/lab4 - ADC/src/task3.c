#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"
#include "adc.h"
#include "task3.h"

// TODO [task3]: Implement the following function to auto-trigger the ADC conversion
void adc_auto_trigger_init(void) {
}

// TODO [task3]: Implement the following function to configure the ADC channel
// HINT: similar to myAnalogRead function in adc.c
void configure_adc_channel(uint8_t channel) {
}

//  TODO [task3]: Interrupt Service Routine for ADC conversion complete
ISR(ADC_vect) {
}

// TODO [task3]: Interrupt Service Routine for Timer1 compare match
ISR(TIMER1_COMPA_vect) {
}

// Timer1 configured to trigger ISR every 1 second
void timer1_init(void) {
  // Set the Timer Mode to CTC
  TCCR1B |= (1 << WGM12);

  // Set the value that you want to count to
  OCR1A = 46875;

  // Start the timer with a prescaler of 256
  TCCR1B |= (1 << CS12);

  // Enable the compare match interrupt
  TIMSK1 |= (1 << OCIE1A);
}

void task3(void) {
  timer1_init();

  while (1) {
    // TODO [task3]: Implement the main loop to read the ADC value of the temperature sensor (PA0)
    // and print it to the serial every 1 second
  }
}