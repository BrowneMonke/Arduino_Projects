#include <avr/io.h>
#include <avr/interrupt.h>

//button 1 = first button
void enableButtonInterrupt(uint8_t button) {
  PORTC = 1 << button;
  PCICR |= _BV(PCIE1); // PORTC interrupts mogelijk maken
  PCMSK1 |= 1 << button; // Pin Change Mask Register
  sei(); // Set Enable Interrupts
}

void enableButton(int button) {
    if ( button < 0 || button > 2 ) {return;}
    DDRC &= ~(1<<(PC1 + button));
    PORTC |= 1<<(PC1 + button); // pull up
}

void enableAllButtons() {
    DDRC &= ~ 0b00001110;
    PORTC |= 0b00001110;
}

int buttonPushed(int button) {
    if (bit_is_clear(PINC, (PC1 + button))) {
        return 1;
    }
    return 0;
}

int buttonReleased(int button) {
    return !buttonPushed(button);
}
