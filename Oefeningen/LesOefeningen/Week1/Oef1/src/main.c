#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"
#include "led.h"

/* #define NUMBER_OF_LEDS 4

//DDR klaarzetten schrijven=1 of lezen=0
void enableLed(int ledNumber){
  if (!(ledNumber < NUMBER_OF_LEDS && ledNumber >= 0)){
    return;
  }
  DDRB |= 0b00000001 << (ledNumber + PB2);
  PORTB |= 0b00000001 << (ledNumber + PB2);

}

void lightUpLed(int ledNumber){
  if (!(ledNumber < NUMBER_OF_LEDS && ledNumber >= 0)){
    return;  
  }
  PORTB &= ~(0b00000001 << (ledNumber + PB2));
}

void lightDownLed(int ledNumber){
  if (!(ledNumber < NUMBER_OF_LEDS && ledNumber >= 0)){
    return;  
  }
  PORTB |= 0b00000001 << (ledNumber + PB2);
}
 */
int main(){
    initUSART();

    int ledNumber = 2;
    enableLed(ledNumber);
    enableLed(3);
    lightUpLed(3);
    //DDRB = 0b00110100; // register schrijven naar binair 5e

    int loopCounter = 0;
    while(1){
      //PORTB |= 0b00000001 <<5;
      //PORTB = 0b00100000; // 5V opzetten -> Geen spanningsverschil dus lamp uit
      lightUpLed(ledNumber);
      _delay_ms(500);
      //PORTB = 0b00000100; // 0V opzetten -> 5V spanningsverschil dus lamp aan
      lightDownLed(ledNumber);
      _delay_ms(500);

      printString("\n in de loop ");
      printf(" %d", ++loopCounter);
    }

  return 0;
}