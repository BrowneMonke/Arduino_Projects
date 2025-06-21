#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "button.h"
#include "display.h"
#include "led.h"
#include "buzzer.h"
#include "usart.h"

//timer 
#define TIMER_MULTIPLIER 250
//game parameters
#define MAX_FUEL_RESERVE 1500    // [L]
#define TOTAL_NO_OF_LEDS 4
#define BLINKER_LED_DOWNTIME 1000  // [ticks]
#define BLINK_FRACTION_MULTIPLICATIOIN_FACTOR 20000
#define STARTING_DISTANCE 9999 // [m]
#define GAME_OVER_DISTANCE 3   // [m]
#define LANDING_SPEED_LIMIT 5  // [m/s]
#define GRAVITY 1.622          // [m/(s*s)]

typedef struct {
  short distance;
  short current_speed;
  uint8_t burst;
  short fuel_reserve;
} LANDING_DATA;

uint8_t game_over = 0;
uint8_t crashed = 0;
uint8_t out_of_fuel = 0;
short current_speed = 100;           // [m/s] -> = (g-burst/5)*t
short distance = STARTING_DISTANCE;  // [m] -> = v*t 
short fuel_reserve = 1500;           // current fuel_reserve;
uint8_t burst = 0;

LANDING_DATA* p_logbook;
uint16_t logbook_index = 0;

uint8_t tick_counter = 0;

void printLandingData() {
    printf("Secod %03d D: %04d; S: %03d; F: %04d\n", logbook_index, distance, current_speed, fuel_reserve);
}

void initTimer() {
    TCCR2A |= (1 << WGM21) | (1 << WGM20);  // CTC Mode voor de counter -> zelf de max tickwaarde bepalen (GEEN Fast PWM)
    TIMSK2 |= (1 << OCIE2A);  // OCRA interrupt enablen
    sei();
}

void startTimer() {
    TCCR2B |= (1 << CS22) | (1 << CS21);  // PRESCALER: 256
    OCR2A = 249;            // na 250 tellen 250 * 16us = 4ms
}

void updateStatus() {
    current_speed += GRAVITY - burst / 5;
    if (distance - current_speed <= STARTING_DISTANCE) {
        distance -= current_speed;
    } 
    else {
        distance = STARTING_DISTANCE;
    }
    if (fuel_reserve > 0) {
        fuel_reserve -= burst;
    }
    if (fuel_reserve <= 0) {
        fuel_reserve = 0;
        out_of_fuel = 1;
    }
    burst = 0;
}

void addStatusToLogbook() {
    (p_logbook + logbook_index)->distance = distance;
    (p_logbook + logbook_index)->current_speed = current_speed;
    (p_logbook + logbook_index)->burst = burst;
    (p_logbook + logbook_index)->fuel_reserve = fuel_reserve;
    logbook_index++;
    printLandingData();
}

ISR(TIMER2_COMPA_vect) {
    if (buttonPushed(1) && burst < 50 && fuel_reserve > 0) {
        burst++;
    }
    if (tick_counter == TIMER_MULTIPLIER) {
        updateStatus();
        addStatusToLogbook();
        if (out_of_fuel) {
            playOutOfFuelTone();
        }
        tick_counter = 0;
    }
    tick_counter++;
}

void blinkFuelLed(uint16_t* p_fuel_blink_counter, uint16_t fuel_blink_value, uint8_t fuel_meter) {
    if (*p_fuel_blink_counter < fuel_blink_value) {
        lightUpLed(3 - fuel_meter);
    } else if (*p_fuel_blink_counter >= fuel_blink_value + BLINKER_LED_DOWNTIME) {
        *p_fuel_blink_counter = 0;
    } else {
        lightDownLed(3 - fuel_meter);
    }
}

void showFuelStatus(uint16_t* p_fuel_blink_counter) {
    lightDownAllLeds();
    float fuel_frac = (float)fuel_reserve / MAX_FUEL_RESERVE;
    float fuel_meter = TOTAL_NO_OF_LEDS * fuel_frac;  // integer value gives the number of LEDs to stay lit up; remainder is the blinking LED value
    uint16_t fuel_blink_value = (fuel_meter - (uint8_t)fuel_meter) * BLINK_FRACTION_MULTIPLICATIOIN_FACTOR;

    for (uint8_t i = 0; i < (uint8_t)fuel_meter; i++) {
        lightUpLed(3 - i);
    }
    blinkFuelLed(p_fuel_blink_counter, fuel_blink_value, fuel_meter);
}

void showDistance() {
    writeNumberToSegment(0, distance/1000);
    writeNumberToSegment(1, (distance/100) % 10);
    writeNumberToSegment(2, (distance/10) % 10);
    writeNumberToSegment(3, distance % 10);
}

void showParameters(uint16_t* p_fuel_blink_counter) {
    showFuelStatus(p_fuel_blink_counter);
    showDistance();
}

void checkGameOver() {
    if (distance <= GAME_OVER_DISTANCE) {
        if (current_speed > LANDING_SPEED_LIMIT) {
            crashed = 1;
        }
        game_over = 1;
    }
}

void playResultTune() {
    if (crashed) {
        playGameLostTune();
    } 
    else {
        playGameWonTune();
    }
}

void endGame() {
    cli();
    writeCharToSegment(1, crashed? 'L' : 'W');
    playResultTune();
    printf("Game Over\n");
}

void playGame() {
    printf("W E L C O M E !\n");
    p_logbook = calloc(200, sizeof(LANDING_DATA));
    uint16_t fuel_blink_counter = 0;
    startTimer();
    while (!game_over) {
        showParameters(&fuel_blink_counter);
        checkGameOver();
        fuel_blink_counter++;
    }
    endGame();
}

void showLogbook() {
    printf("\n\n\n   LOGBOOK\n-------------\n");
    for (uint8_t i = 0; i < logbook_index; i++) {
        printf("Second %03d -> Distance remaining: %04d ; Speed: %03d ; Burst: %02d ; Fuel reserve: %04dL\n", i+1, (p_logbook+i)->distance, (p_logbook+i)->current_speed, (p_logbook+i)->burst, (p_logbook+i)->fuel_reserve);
    }
    printf("\n\n");
}

void freeMem() {
    free(p_logbook);
}

int main() {
    initDisplay();
    initUSART();
    initTimer();
    enableBuzzer();
    enableAllLeds();
    enableAllButtons();

    playGame();
    showLogbook();
    freeMem();

    return 0;
}