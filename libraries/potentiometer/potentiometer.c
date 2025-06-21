#include <avr/io.h>

#include "display.h"

void initADC() {
  ADMUX |= (1<<REFS0); //instellen van de voltage (bij ons 5V)
  ADCSRA |= (1<<ADEN); //enable ADC
  ADCSRA |= (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2); //<=> ADC |= 0b111 => set division factor to 128
}

void showAdcValue(uint16_t adc_value) {
    writeNumberAndWait(adc_value, 500);
}