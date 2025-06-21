#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <usart.h>
#include <led.h>

ISR(USART_RX_vect)//De interrupt routine voor het ontvangen van data
{
    uint8_t byte = UDR0;//seriële data zit in UDR0 register
    //flipLed(1);//flipled om indicatie te geven dat we iets ontvangen hebben
    printf("%c",byte+1);//voor dit demootje sturen we dezelfde byte verhoogt met 1 terug...
}

int main(){
  initUSART();
  enableLed(0);enableLed(1);
  UCSR0B |= (1 << RXCIE0);//dit zorgt ervoor dat er een interrupt afgaat zodra er data over serieel ontvangen wordt
  sei();
  while (1) {
    //flipLed(0);//heartbeat...
    _delay_ms(1000);
  }
  return 0;
}