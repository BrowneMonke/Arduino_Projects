#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

// #include "button.h"
#include "display.h"
#include "led.h"
#include "usart.h"

ISR(TIMER0_COMPA_vect) { // OCR0A (nu 127)
	lightUpLed(0);
}
ISR(TIMER0_OVF_vect) { //timer 0 dus OVF = overflow = 255
	lightDownLed(0);
}

int main() {
    initDisplay();
    initUSART();
	//enableLeds(0b00110000);
	enableAllLeds();
	sei();

    TCCR0A |= (1 << WGM00) | (1 << WGM01);  // WGM00 WGM01 op 1 --> FAST PWM Mode
    TCCR0B |= (1 << CS00)  | (1 << CS02); 	// PRESCALER: 1024 => timer vertragen door de tickwaarde te delen door 1024 (na 1024 ticks v.d. processor -> 1 tick v.d. timer)

	TIMSK0 |= (1 << OCIE0A);	// OCRA interrupt enablen
	TIMSK0 |= (1 << TOIE0);		// overflow(op 255) interrupt enablen
	OCR0A = 250;
	//TIMSK -> Timer/Counter Interrupt Mask Register
	while (1) {
		printf("\nGeef de OCR0A waarde (tussen 0 en 255): ");
		OCR0A = getNumber();
	}
	
	/* 
    for (size_t i = 0; i < 30; i++) {
		printf("TCNT0 %d\n", TCNT0);
    } */

    return 0;
}