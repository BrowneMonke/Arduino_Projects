#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>

#include "button.h"
#include "display.h"
#include "led.h"
#include "potentiometer.h"
#include "usart.h"

#define min_starting_amount 21

uint8_t starting_amount;
uint8_t max_amount;

uint8_t available_matches;
uint8_t picked_number_of_matches;
char turn = 'C';
char** p_game_history;

void changeTurn() { turn = (turn == 'C') ? 'P' : 'C'; }


void showSeed(uint16_t seed) {
    uint16_t delay = 4000;
    lightUpAllLeds();
    uint8_t led_number = 0;
    for (int i = 0; i < delay / 20; i++) {
        writeNumberToSegment(0, seed / 1000);
        _delay_ms(5);
        writeNumberToSegment(1, (seed / 100) % 10);
        _delay_ms(5);
        writeNumberToSegment(2, (seed / 10) % 10);
        _delay_ms(5);
        writeNumberToSegment(3, seed % 10);
        _delay_ms(5);
        if (i != 0 && i % (delay/80) == 0) {
            lightDownLed(led_number++);
        }
    }
    lightDownLed(led_number);
}

void checkStartButton(uint16_t adc_value, uint8_t* p_game_start) {
    if (buttonPushed(0)) {
        double adc_fraction = adc_value/1023.0;
        uint16_t seed = (uint16_t) 9999*adc_fraction;
        srand(seed);
        if (rand()%2 == 1) {changeTurn();}//ADC Value = 396 gives C first; ADC = 409 gives P
        (*p_game_start) = 1;
        starting_amount = (uint8_t) (min_starting_amount + 78 * adc_fraction);
        available_matches = starting_amount;
        max_amount = (uint8_t) (3 + 6 * adc_fraction);
        picked_number_of_matches = max_amount;
        p_game_history = calloc(available_matches, sizeof(char*));
        printf("ADC Value: %d\nSEED: %d; max amount: %d; starting amount: %d\n", adc_value, seed, max_amount, available_matches);
        showSeed(seed);
        writeStringAndWait("    ", 100);
    }
}

void waitForStart() {
    uint8_t game_start = 0;
    uint16_t adc_value;
    while(!game_start) {
        ADCSRA |= (1<<ADSC);
        loop_until_bit_is_clear(ADCSRA, ADSC);
        adc_value = ADC;   
        showAdcValue(adc_value);
        checkStartButton(adc_value, &game_start);
    }
}

void addGameHistory(uint8_t* p_index) {
    char model_string[] = "P: picked matches 9; available matches 90";
    *(p_game_history + (*p_index)) = calloc(strlen(model_string)+1, sizeof(char));
    snprintf((*(p_game_history + (*p_index))), strlen(model_string)+1, "%c: picked matches %d; available matches %2d", turn, picked_number_of_matches, available_matches);
    (*p_index)++;
}

void checkButtonPushed(uint8_t button, uint8_t* p_picked_no_of_matches, uint8_t* p_index) {
    _delay_ms(50);
    if (buttonPushed(button)) {
        printf("Pressed button %d\n", button);
        if (button == 2 && (*p_picked_no_of_matches) < max_amount) {
            (*p_picked_no_of_matches)++;
        } else if (button == 0 && (*p_picked_no_of_matches) > 1) {
            (*p_picked_no_of_matches)--;
        } else if (button == 1) {
            if (available_matches > (*p_picked_no_of_matches)) {
                available_matches -= (*p_picked_no_of_matches);
                printf("%c: picked matches %d; available matches %d\n", turn, picked_number_of_matches, available_matches);
                addGameHistory(p_index);
                changeTurn();
            }
        }
    }
}

void readInput(uint8_t* p_picked_no_of_matches, uint8_t* p_index) {
    if (buttonPushed(0)) {
        checkButtonPushed(0, p_picked_no_of_matches, p_index);
    } else if (buttonPushed(1)) {
        checkButtonPushed(1, p_picked_no_of_matches, p_index);
    } else if (buttonPushed(2)) {
        checkButtonPushed(2, p_picked_no_of_matches, p_index);
    }
}

void checkGameOver(uint8_t* p_game_over) {
    if (available_matches == 1) {
        printf("Game Over! The winner is: %c\n", turn == 'C' ? 'P' : 'C');
        (*p_game_over) = 1;
    }
    else if (available_matches == 0) {
        printf("Game Over! The winner is: %c\n", turn);
        (*p_game_over) = 1;
    }
}

void blinkPlayerLed(short* p_blink_counter) {
    if ((*p_blink_counter) < 4500) {
        writeCharToSegment(1, turn);
    }
    else if ((*p_blink_counter) == 10000) {
        (*p_blink_counter) = 0;
    }
}

void displayValues() {
    uint8_t am_ones_value = available_matches % 10;
    uint8_t am_tens_value = (available_matches / 10);

    writeNumberToSegment(3, am_ones_value);
    writeNumberToSegment(2, am_tens_value);
    writeNumberToSegment(0, picked_number_of_matches);
}

void computerMove(uint8_t* p_index) {
    uint8_t pick_amount = (available_matches - 1) % (max_amount + 1);
    if (pick_amount == 0) {
        picked_number_of_matches = (rand() % 3) + 1;
    }
    else {
        picked_number_of_matches = pick_amount;
    }
    available_matches -= picked_number_of_matches;
    printf("%c: picked matches %d; available matches %d\n", turn, picked_number_of_matches, available_matches);
    writeStringAndWait("    ", 500);
    for (short i = 0; i < 30000; i++) {
        displayValues();
        writeCharToSegment(1, turn);
    }
    addGameHistory(p_index);
    changeTurn();
    picked_number_of_matches = max_amount;
}

void showHistory(uint8_t* p_index) {
    printf("\n   HISTORY   \n-------------\nMatches available at start: %2d\n", starting_amount);
    for (uint8_t i = 0; i < (*p_index); i++) {
        printf("Turn %2d -> %s\n", i+1, *(p_game_history+i));
    }
}

void freeMem(uint8_t index) {
    for (uint8_t i = 0; i < index; i++) {
        free(*(p_game_history+i));
    }
    free(p_game_history);
}

void playGame() {
    uint8_t index = 0;
    uint8_t game_over = 0;
    short blink_counter = 0;
    while (!game_over) {
        blinkPlayerLed(&blink_counter);
        displayValues();
        if (turn == 'P') {
            readInput(&picked_number_of_matches, &index);
        }
        else {
            computerMove(&index);
        }
        checkGameOver(&game_over);
        blink_counter++;
    }
    showHistory(&index);
    freeMem(index);
}

int main() {
    initDisplay();
    initUSART();
    initADC();
    enableAllLeds();

    waitForStart();
    playGame();
    return 0;
}