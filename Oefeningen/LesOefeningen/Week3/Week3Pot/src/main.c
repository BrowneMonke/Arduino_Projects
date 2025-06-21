#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "led.h"
#include "button.h"
#include "display.h"

void initADC() {
  //ADMUX |= (1); //button 1 (PC1) als input: 0b00000001 => PC0 = 1 = disabled dus de eerstvolgende pin met waarde 0 (PC1 in dit geval) gelezen
  ADMUX |= (1<<REFS0); //instellen van de voltage (bij ons 5V)
  ADCSRA |= (1<<ADEN); //enable ADC
  ADCSRA |= (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2); //<=> ADC |= 0b111 => set division factor to 128
}

int main() {
  initDisplay();
  int adcValue;
  initUSART();
  initADC();

  while(1) {
    ADCSRA |= (1<<ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
    adcValue = ADC;
    printf("ADC Value: %d\n", adcValue);
    writeNumberAndWait(adcValue, 1000);
    //_delay_ms(3000);
  }

}

