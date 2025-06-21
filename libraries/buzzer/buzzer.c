#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/io.h>
#include <util/delay.h>

// FREQUENTIES VAN DE VERSCHILLENDE NOTEN
#define C5 523.250
#define D5 587.330
#define E5 659.250
#define F5 698.460
#define G5 783.990
#define A5 880.000
#define B5 987.770
#define C6 1046.500
#define HIGH_PITCH_FREQ_MULTIP 4
#define LOW_PITCH_FREQ_MULTIP 1

const float frequencies[] = {C5, D5, E5, F5, G5, A5, B5, C6};  // do-re-mi-fa-sol-la-si-do

void enableBuzzer() { // buzzer hangt op PD3
	DDRD |= (1 << PD3); 
	PORTD |= (1 << PD3);
}  

// Enter duration in ms
void playTone(float frequency, uint8_t frequency_multiplier, uint32_t duration) {
    uint32_t period_in_micro = (uint32_t)(1000000 / (frequency*frequency_multiplier));  // we berekenen de periode in microseconden uit de frequency
    uint32_t duration_in_micro = duration * 1000;  	// we willen duration in microseconden
    for (uint32_t time = 0; time < duration_in_micro; time += period_in_micro) { 
        PORTD &= ~(1 << PD3);      		// aanzetten van de buzzer
        _delay_us(period_in_micro / 2); 	// helft van de periode wachten
        PORTD |= (1 << PD3);            // afzetten van de buzzer
        _delay_us(period_in_micro / 2);   // weer helft van de periode wachten
    }
}

// Enter duration in ms
void playSequence(uint32_t duration) {
	for (uint8_t i = 0; i < 8; i++) {
		playTone(frequencies[i], HIGH_PITCH_FREQ_MULTIP, duration);
	}
}

// Enter duration in ms
void playSequenceBackwards(uint32_t duration) {
	for (short i = 7; i >= 0; i--) {
		playTone(frequencies[i], HIGH_PITCH_FREQ_MULTIP, duration);
	}
}

void playGameLostTune() {
	playTone(frequencies[1], LOW_PITCH_FREQ_MULTIP, 250);
	playTone(frequencies[0], LOW_PITCH_FREQ_MULTIP, 500);
}

void playGameWonTune() {
	playTone(frequencies[5], LOW_PITCH_FREQ_MULTIP, 350);
	playTone(frequencies[6], LOW_PITCH_FREQ_MULTIP, 200);
	playTone(frequencies[7], LOW_PITCH_FREQ_MULTIP, 500);
}

void playCounterTone() {
	playTone(frequencies[0], HIGH_PITCH_FREQ_MULTIP, 70);
}

void playCounterFinishedTone() {
	playTone(frequencies[3], HIGH_PITCH_FREQ_MULTIP, 120);
}

void playPromptInputTone() {
	playTone(frequencies[0], HIGH_PITCH_FREQ_MULTIP, 50);
	_delay_ms(35);
	playTone(frequencies[0], HIGH_PITCH_FREQ_MULTIP, 50);
}

void playOutOfFuelTone() {
	playTone(frequencies[0], LOW_PITCH_FREQ_MULTIP, 300);
}