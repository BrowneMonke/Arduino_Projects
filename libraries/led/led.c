#include <avr/io.h>
#include <util/delay.h>

#define NUMBER_OF_LEDS 4

void enableLed(int ledNumber){
  if (!(ledNumber < NUMBER_OF_LEDS && ledNumber >= 0)){
    return;
  }
  DDRB |= 0b00000001 << (PB5 - ledNumber);
  PORTB |= 0b00000001 << (PB5 - ledNumber);
}
void enableLeds(uint8_t ddrbState){
  DDRB |= ddrbState;
  PORTB |= ddrbState;
}
void enableAllLeds(){
  DDRB |= 0b00111100;
  PORTB |= 0b00111100;
}

void lightUpLed(int ledNumber){
  if (!(ledNumber < NUMBER_OF_LEDS && ledNumber >= 0)){
    return;  
  }
  PORTB &= ~(0b00000001 << (PB5 - ledNumber));
}
void lightUpLeds (uint8_t pbState){
  PORTB &= ~(pbState);
}
void lightUpAllLeds(){
  PORTB &= 0b11000011;
}

void lightDownLed(int ledNumber){
  if (!(ledNumber < NUMBER_OF_LEDS && ledNumber >= 0)){
    return;  
  }
  PORTB |= 0b00000001 << (PB5 - ledNumber);
}
void lightDownLeds (uint8_t pbState){
  PORTB |= pbState;
}
void lightDownAllLeds(){
    PORTB |= 0b00111100;
}

void countdownLights(){
    lightUpAllLeds();
    for (short i = 0; i < 4; i++) {
        _delay_ms(1000);
        lightDownLed(i);
    }
    _delay_ms(1000);
}
void ledDance(){
    for (short i = 0; i < 2; i++) {
        for (short j = 0; j < 4; j++) {
            lightUpLed(j);
            _delay_ms(100);
            lightDownLed(j);
        }
        for (short j = 3; j >= 0; j--) {
            lightUpLed(j);
            _delay_ms(100);
            lightDownLed(j);
        }
    }
}