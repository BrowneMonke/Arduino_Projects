#include <avr/io.h>

void initTimer();
void startTimer();
void stopTimer();
void resetTimer();
uint8_t getSeconds();
uint8_t getMinutes();
