#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#include "button.h"
#include "display.h"
#include "led.h"
#include "usart.h"

int main() {
  DDRD |= (1<<PD3); //enable buzzer
  enableLed(1);
  uint32_t periode = (uint32_t) (1000000/880);

  for (short i = 0; i < periode*4; i+=periode/2) {
    lightUpLed(1);
    PORTD &= ~(1 << PD3); //buzzer aanzetten (value = 0)
    _delay_us(periode/2);
    lightDownLed(1);
    PORTD |= (1 << PD3); // buzzer uitzetten (value = 1)
    _delay_us(periode/2);
  }


  return 0;
}