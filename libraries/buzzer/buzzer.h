#include <avr/io.h>
#include <stdlib.h>

void enableBuzzer();

void playTone(float frequency, uint8_t frequency_multiplier, uint32_t duration);

void playSequence(uint32_t duration);
void playSequenceBackwards(uint32_t duration);

void playGameWonTune();
void playGameLostTune();

void playCounterTone();
void playCounterFinishedTone();
void playPromptInputTone();

void playOutOfFuelTone();