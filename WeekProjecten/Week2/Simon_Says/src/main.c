#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "led.h"
#include "button.h"
 

#define DELAY _delay_ms(500)
#define MAX_TURNS 3

//Global variables
int seed = 108;
uint8_t array_length = 1;
uint8_t start_button_pushed = 0;
uint8_t game_over = 0;
uint8_t game_won = 0;


ISR(PCINT1_vect) {
  if (buttonPushed(0)) {
    start_button_pushed = 1;
  }
}

void enableButtonInterrupt() {
  cli();
  PORTC = 1 << PC1;
  PCICR |= _BV(PCIE1); // PORTC interrupts mogelijk maken
  PCMSK1 |= 1 << PC1; // Pin Change Mask Register
  sei(); // Set Enable Interrupts
}

void waitForStart() {
  while(!start_button_pushed) {
    lightUpLed(3);
    _delay_ms(100);
    lightDownLed(3);
    _delay_ms(100);
    seed++;
  }
}

void generatePuzzle(uint8_t led_numbers[array_length]) {
  srand(seed);
  for (uint8_t i = 0; i < array_length; i++) {
    led_numbers[i] = rand() % 3;
  }
}

void printPuzzle(uint8_t led_numbers[array_length]) {
  printf("[");
  for (uint8_t i = 0; i < array_length; i++) { 
    printf("%d ", led_numbers[i]);
  }
  printf("\b]\n");
}

void playPuzzle(uint8_t led_numbers[array_length]) {
  countdownLights();
  for (short i = 0; i < array_length; i++) {
    lightUpLed(led_numbers[i]);
    DELAY;
    lightDownLed(led_numbers[i]);
    DELAY;
  }
}

void checkButtonPushed(uint8_t led_number[array_length], uint8_t button, uint8_t* p_index) {
  _delay_ms(50);
  if (buttonPushed(button)) {
    printf("Pressed button %d\n", button);
    if (button != led_number[*p_index]) {
      game_over = 1;
    }
    (*p_index)++;
  }
}

void readInput(uint8_t led_number[array_length]) {
  uint8_t index = 0;
  while (!game_over && index < array_length) {
    if (buttonPushed(0)) {
      checkButtonPushed(led_number, 0, &index);
    }
    else if (buttonPushed(1)) {
      checkButtonPushed(led_number, 1, &index);
    }
    else if (buttonPushed(2)) {
      checkButtonPushed(led_number, 2, &index);
    }
  }
  printf("\n");
}

void showGameWonLight() {
  for (uint8_t i = 0; i < 21; i++) {
    lightUpAllLeds();
    _delay_ms(100);
    lightDownAllLeds();
    _delay_ms(100);
  }
}

void showGameOverLight() {
  lightUpAllLeds();
  _delay_ms(5000);
  lightDownAllLeds();
}

void printResult() {
  if (game_won) {
    printf("WINNER WINNER CHICKEN DINNER!!\n\n");
    showGameWonLight();
    ledDance();
  }
  else if (game_over) {
    printf("GAME OVER!! Better luck next time! :')\n\n");
    showGameOverLight();
  }
  else {
    printf("ERRORRRR!!");
  }
}

void playGame() {
  while (!(game_over || game_won)){
    uint8_t led_numbers[array_length];
    generatePuzzle(led_numbers);
    playPuzzle(led_numbers);
    printPuzzle(led_numbers);
    readInput(led_numbers);
    array_length++;
  
    if (array_length == MAX_TURNS + 1 && !game_over) {//!game_over is necessary here because array_length increases regardless of game_over value
      game_won = 1;
    }
  }
}


int main() {
  initUSART();

  enableAllLeds();
  enableAllButtons();
  enableButtonInterrupt();

  waitForStart();
  playGame();
  printResult();

  return 0;
}