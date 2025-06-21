#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "button.h"
#include "display.h"
#include "led.h"
#include "usart.h"

// globale variabelen
#define VEELVOUD 250
volatile int counter = 0;
volatile int sec = 0;
volatile int min = 0;


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
	if (sec >= 60) {
		min++;
		sec = 0;
	}
    printf("tijd: %d:%02d\n", min, sec);
}

void writeTimeAndWait(uint8_t minuten, uint8_t seconden, int delay) {
    // Vul aan
}

// Deze ISR loopt elke 4ms
ISR(TIMER2_COMPA_vect) {
	counter++;
	if (counter == VEELVOUD) {
		sec++;
		counter = 0;
		tick();
	}
}

ISR(PCINT1_vect) {
	if (buttonPushed(0)) {
		printf("Start timer\n");
		startTimer();
	}
	else if (buttonPushed(1)) {
		printf("Stop timer\n");
		stopTimer();
	}
	else if (buttonPushed(2)) {
		printf("Reset timer\n");
		resetTimer();
	}
}

void initButtonInterrupts() {
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PC1) | (1 << PC2) | (1 << PC3);
}

int main() {
    initUSART();
    initDisplay();
	initTimer();
	initButtonInterrupts();
	enableAllButtons();

    printf("Start de stopwatch met S1, stop met S2, en reset met S3\n");
	//startTimer();

    while (1) {
        // vergeet je timer niet te initialiseren en te starten
        // denk ook aan sei()
    }

    return 0;
}