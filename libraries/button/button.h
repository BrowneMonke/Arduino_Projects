#include <avr/io.h>

void enableButtonInterrupt(uint8_t button);


void enableButton(int button);
void enableAllButtons();

int buttonPushed(int button);
int buttonReleased(int button);