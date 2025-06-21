
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

#define VEELVOUD 250

// globale variabelen
volatile uint8_t counter = 0;
volatile uint8_t sec = 0;
volatile uint8_t min = 0;


void initTimer() {
    TCCR2A |= (1 << WGM21) | (1 << WGM20);  // CTC Mode voor de counter -> zelf de max tickwaarde bepalen (GEEN Fast PWM)
    TIMSK2 |= (1 << OCIE2A);  // OCRA interrupt enablen
    sei();
}

void startTimer() {
    TCCR2B |= (1 << CS22) | (1 << CS21);  // PRESCALER: 256
    OCR2A = 249;            // na 250 tellen 250 * 16us = 4ms
}

void stopTimer() {
    TCCR2B = 0;		// PRESCALER: 0
}

void resetTimer() {
	sec = 0;
	min = 0;
}

void tick() {
	sec++;
	if (sec >= 60) {
		min++;
		sec = 0;
	}
    //printf("tijd: %d:%02d\n", min, sec);
}

// Deze ISR loopt elke 4ms
ISR(TIMER2_COMPA_vect) {
	counter++;
	if (counter == VEELVOUD) {
		tick();
		counter = 0;
	}
}

uint8_t getSeconds() {
    return sec;
}

uint8_t getMinutes() {
    return min;
}
