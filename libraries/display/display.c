#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "display.h"

#define SPACE 0xFF

/* Segment byte maps for numbers 0 to 9 */
const uint8_t SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
                               0x92, 0x82, 0xF8, 0X80, 0X90};

/* Segment byte maps for the alphabet */
const uint8_t SEGMENT_ALPHA[] = {
    0x88 /*A*/, 0x83 /*b*/, 0xC6 /*C*/, 0xA1 /*d*/, 0x86 /*E*/, 0x8E /*F*/,
    0xC2 /*G*/, 0x89 /*h*/, 0xCF /*I*/, 0xE1 /*J*/, 0x8A /*k*/, 0xC7 /*L*/,
    0xEA /*M*/, 0xC8 /*n*/, 0xC0 /*O*/, 0x8C /*P*/, 0x4A /*q*/, 0xCC /*r*/,
    0x92 /*S*/, 0x87 /*t*/, 0xC1 /*U*/, 0xC1 /*V*/, 0xD5 /*W*/, 0x89 /*X*/,
    0x91 /*y*/, 0xA4 /*Z*/};

/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

void initDisplay() {
    sbi(DDRD, LATCH_DIO);
    sbi(DDRD, CLK_DIO);
    sbi(DDRB, DATA_DIO);
}

// loop through seven segments of LED display and shift the correct bits in the
// data register
void shift(uint8_t val, uint8_t bitorder) {
    uint8_t bit;

    for (uint8_t i = 0; i < NUMBER_OF_SEGMENTS; i++) {
        if (bitorder == LSBFIRST) {
            bit = !!(val & (1 << i));
        } else {
            bit = !!(val & (1 << (7 - i)));
        }
        // write bit to register
        if (bit == HIGH)
            sbi(PORTB, DATA_DIO);
        else
            cbi(PORTB, DATA_DIO);

        // Trigger the clock pin so the display updates
        sbi(PORTD, CLK_DIO);
        cbi(PORTD, CLK_DIO);
    }
}

// Schrijft cijfer naar bepaald segment. Segment 0 is meest linkse.
void writeNumberToSegment(uint8_t segment, uint8_t value) {
    cbi(PORTD, LATCH_DIO);
    shift(SEGMENT_MAP[value], MSBFIRST);
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
}

// Schrijft getal tussen 0 en 9999 naar de display. Te gebruiken in een lus...
void writeNumber(int number) {
    if (number < 0 || number > 9999) return;
    writeNumberToSegment(0, number / 1000);
    writeNumberToSegment(1, (number / 100) % 10);
    writeNumberToSegment(2, (number / 10) % 10);
    writeNumberToSegment(3, number % 10);
}

// Schrijft getal tussen 0 en 9999 naar de display en zorgt dat het er een
// bepaald aantal milliseconden blijft staan. Opgelet: de timing is "ongeveer",
// er wordt geen rekening gehouden met de tijd writeNumberToSegment in beslag
// neemt...
void writeNumberAndWait(int number, int delay) {
    if (number < 0 || number > 9999) return;
    for (int i = 0; i < delay / 20; i++) {
        writeNumberToSegment(0, number / 1000);
        _delay_ms(5);
        writeNumberToSegment(1, (number / 100) % 10);
        _delay_ms(5);
        writeNumberToSegment(2, (number / 10) % 10);
        _delay_ms(5);
        writeNumberToSegment(3, number % 10);
        _delay_ms(5);
    }
}

// Schrijft letter naar bepaald segment. Segment 0 is meest linkse.
void writeCharToSegment(uint8_t segment, char character) {
    cbi(PORTD, LATCH_DIO);
    if (character == ' ') {
        shift(SPACE, MSBFIRST);
    } else {
        if (character > 96 && character < 123) {
            character = character - 32;
        }
        shift(SEGMENT_ALPHA[character - 'A'], MSBFIRST);
    }
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
}

// Schrijft woord van max lengte 4 naar de display. Te gebruiken in een lus...
void writeString(char* str) {
    if (strlen(str) > 5) return;
    writeCharToSegment(0, *str);
    writeCharToSegment(1, *(str + 1));
    writeCharToSegment(2, *(str + 2));
    writeCharToSegment(3, *(str + 3));
}

void writeStringAndWait(char* str, int delay) {
    if (strlen(str) > 5) return;

    for (int i = 0; i < delay / 20; i++) {
      writeCharToSegment(0, *str);
      _delay_ms(5);
      writeCharToSegment(1, *(str + 1));
      _delay_ms(5);
      writeCharToSegment(2, *(str + 2));
      _delay_ms(5);
      writeCharToSegment(3, *(str + 3));
      _delay_ms(5);
    }
}