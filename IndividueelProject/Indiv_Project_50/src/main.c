#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "button.h"
#include "buzzer.h"
#include "display.h"
#include "led.h"
#include "potentiometer.h"
#include "timer.h"
#include "usart.h"


#define MAX_NO_OF_TURNS 100
#define STARTING_POINTS 50
#define MIN_ROLL_VALUE 1
#define MAX_TOTAL_COMPUTER_ROLL_VALUE 17
#define BUTTON_DELAY_MS 70
#define NOTE_DURATION_MS 250

typedef struct
{
    uint8_t points[MAX_NO_OF_TURNS];
    char name;
} PLAYER;

typedef struct
{
    uint8_t points;
    char name;
    uint8_t duration_in_m;
    uint8_t duration_in_s;
} PLAYER_TURN_RECORD;

uint8_t max_roll_value = 7;

uint8_t game_started = 0;
uint8_t game_over = 0;

uint8_t roll_number = 0;
uint8_t history_index = 0;

PLAYER* player_computer;
PLAYER* player_me;
PLAYER* current_player;
PLAYER* winner;

PLAYER_TURN_RECORD** p_game_history;


ISR(PCINT1_vect) {
  if (buttonPushed(1)) {
    printf("---------------------------\nUser has ended the session.\n---------------------------\n");
    game_over = 1;
  }
}

PLAYER* newPlayer(char name) {
    PLAYER* this = calloc(1, sizeof(PLAYER));
    this->points[0] = (uint8_t) STARTING_POINTS;
    this->name = name;
    return this;
}

PLAYER_TURN_RECORD* newPlayerTurnRecord(char name, uint8_t points) {
    PLAYER_TURN_RECORD* this = calloc(1, sizeof(PLAYER_TURN_RECORD));
    this->points = points;
    this->name = name;
    this->duration_in_m = getMinutes();
    this->duration_in_s = getSeconds();
    return this;
}

void addGameHistory(uint8_t points) {
    *(p_game_history + history_index) = calloc(1, sizeof(PLAYER_TURN_RECORD));
    *(p_game_history + history_index) = newPlayerTurnRecord(current_player->name, points);

    history_index++;
}

void changeTurn(uint8_t points, uint8_t beepOnCountdown) {
    if (game_started) {
        if (!game_over) {
            addGameHistory(points);
            printf("Turn duration: %02d min %02d sec\n----------\n", getMinutes(), getSeconds());
        }
    }
    if (!game_over) {
        current_player = (current_player == player_computer) ? player_me : player_computer;
        printf("%c's turn!\n", current_player->name);
        if (current_player == player_me && !beepOnCountdown) {
            playPromptInputTone();
        }
    }
    resetTimer();
}

void setUpPlayers() {
    player_computer = newPlayer('C');
    player_me = newPlayer('P');
    current_player = player_computer;
    if (rand()%2 == 1) {
        changeTurn(0, 0);
    } 
    else {
        printf("C's turn!\n");
    }
}

void checkStartButton(uint16_t adc_value) {
    if (buttonPushed(0)) {
        uint16_t seed = adc_value;//ADC Value = 438 gives C first; ADC = 435 gives P
        writeStringAndWait("    ", 500);
        playSequence(NOTE_DURATION_MS);
        srand(seed);
        p_game_history = calloc(2*MAX_NO_OF_TURNS, sizeof(PLAYER_TURN_RECORD*));
        printf("Max Roll Value: %d\n", max_roll_value);
        setUpPlayers();
        game_started = 1;
        enableButtonInterrupt(2);
        startTimer();
    }
}

void blinkNumber(uint16_t* p_blink_counter, uint8_t number) {
    if ((*p_blink_counter) < 8500) {
        writeNumberToSegment(0, number);
    }
    else if ((*p_blink_counter) > 18000) {
        (*p_blink_counter) = 0;
    }
    else {
        writeCharToSegment(0, ' ');
    }
    (*p_blink_counter)++;
}

void checkIncreaseMaxThrowValueButton() {
    if (buttonPushed(2)) {
        _delay_ms(BUTTON_DELAY_MS);
        if (buttonPushed(2)) {
            if (max_roll_value < 9) {
                max_roll_value++;
            }
        }
    }
}

void checkDecreaseMaxThrowValueButton() {
    if (buttonPushed(0)) {
        _delay_ms(BUTTON_DELAY_MS);
        if (buttonPushed(0)) {
            if (max_roll_value > MIN_ROLL_VALUE+2) {
                max_roll_value--;
            }
        }
    }
}

void checkSelectionMadeButton(uint8_t* p_selection_made) {
    if (buttonPushed(1)) {
        _delay_ms(BUTTON_DELAY_MS);
        if (buttonPushed(1)) {
            *p_selection_made = 1;
        }
    }
}

void checkChangeMaxThrowValueButton() {
    if (buttonPushed(1)) {
        _delay_ms(BUTTON_DELAY_MS);
        if (buttonPushed(1)) {
            _delay_ms(BUTTON_DELAY_MS);
            uint8_t selection_made = 0;
            uint16_t blink_counter = 0;
            while(!selection_made) {
                blinkNumber(&blink_counter, max_roll_value);
                checkIncreaseMaxThrowValueButton();
                checkDecreaseMaxThrowValueButton();
                checkSelectionMadeButton(&selection_made);
            }
        }
    }
}

void waitForStart() {
    printf("\nWelcome! Rotate the potentiometer and hold button 1 to start the game. Release button when display is blank.\nPress button 2 to see dice options (switch between dice with buttons 1 & 3).\nHave fun!! :)\n\n");
    uint16_t adc_value;
    while(!game_started) {
        ADCSRA |= (1<<ADSC);
        loop_until_bit_is_clear(ADCSRA, ADSC);
        adc_value = ADC;
        showAdcValue(adc_value);
        checkStartButton(adc_value);
        checkChangeMaxThrowValueButton();
    }
}


void displayValues(uint8_t* p_points_index, uint8_t die_value) {
    uint8_t points = current_player->points[*p_points_index];
    
    writeNumberToSegment(3, points % 10);
    writeNumberToSegment(2, (points / 10) % 10);
    writeCharToSegment(1, current_player->name);
    writeNumberToSegment(0, die_value);
}

uint8_t rollDie() {
    return (rand() % max_roll_value) + 1;
}

void checkGameOver(uint8_t* p_points_index) {
    if (current_player->points[*p_points_index] == 0) {
        game_over = 1;
        addGameHistory(current_player->points[*p_points_index]);
        printf("Turn duration: %02d min %02d sec\n----------\n", getMinutes(), getSeconds());
        printf("Game Over: Player %c Wins!!", current_player->name);
        winner = current_player;
    }
}

void buzzerBeep(uint8_t beepOnCountdown) {
    if (current_player == player_computer && beepOnCountdown) {
        playCounterTone();
    }
}

void buzzerRollBeep(uint8_t beepOnCountdown) {
    if (current_player == player_computer && beepOnCountdown) {
        playCounterFinishedTone();
    }
}

void displayLastRoll(uint8_t* p_points_index, uint8_t die_value, uint8_t beepOnCountdown) {
    lightUpAllLeds();
    uint8_t led_number = 0;
    uint16_t delay_in_ticks = 10000;
    for (uint16_t i = 0; i < delay_in_ticks; i++) {
        displayValues(p_points_index, die_value);
        if (i != 0 && i % (delay_in_ticks/4) == 0) {
            lightDownLed(led_number++);
            buzzerBeep(beepOnCountdown);
        }
    }
    lightDownLed(led_number);
    buzzerRollBeep(beepOnCountdown);
}

void concludeRoll(uint8_t* p_points_index, uint8_t rolled_value, uint8_t new_points, uint8_t beepOnCountdown) {
    current_player->points[++(*p_points_index)] = new_points; //need to set new points BEFORE changeTurn()
    printf("Roll %02d -> %c: rolled value: %d ; resulting points: %02d\n", ++roll_number, current_player->name, rolled_value, new_points);
    displayLastRoll(p_points_index, rolled_value, beepOnCountdown);
    checkGameOver(p_points_index);
}

void checkTurnEnd(uint8_t* p_points_index, uint8_t* p_new_points, uint8_t die_value, uint8_t rolled_value) {
    if ((rolled_value == MIN_ROLL_VALUE || rolled_value == max_roll_value)) {
        if (rolled_value == max_roll_value) {
            *p_new_points += (die_value)/2;    
        }
        else if (*p_new_points == MIN_ROLL_VALUE) { //IF rolled_value != max_roll_value THEN rolled_value MUST BE = MIN_ROLL_VALUE
            *p_new_points -= rolled_value;
        }
        concludeRoll(p_points_index, rolled_value, *p_new_points, 1); //Need to conclude roll BEFORE changeTurn()
        changeTurn(*p_new_points, 1);
    }
    else {
        if (rolled_value <= *p_new_points) {
            *p_new_points -= rolled_value;
            concludeRoll(p_points_index, rolled_value, *p_new_points, 0);
        }
        else{
            concludeRoll(p_points_index, rolled_value, *p_new_points, 1);   
            changeTurn(*p_new_points, 1);
        }
    }
}

void playTurn(uint8_t* p_die_value, uint8_t* p_points_index) {
    uint8_t new_points = current_player->points[*p_points_index];
    uint8_t rolled_value = rollDie();
    checkTurnEnd(p_points_index, &new_points, *p_die_value, rolled_value);
    *p_die_value = rolled_value;
}

void readInput(uint8_t* p_die_value, uint8_t* p_points_index) {
    if (buttonPushed(0)) {
        _delay_ms(BUTTON_DELAY_MS);
        if (buttonPushed(0)) {
            playTurn(p_die_value, p_points_index);
        }
    }
    if (buttonPushed(2)) {
        _delay_ms(BUTTON_DELAY_MS);
        if (buttonPushed(2)) {
            changeTurn(current_player->points[*p_points_index], 0);
        }
    }
}

void computerMove(uint8_t* p_die_value, uint8_t* p_points_index, uint8_t* p_total_computer_roll_value) {
    playTurn(p_die_value, p_points_index);
    if (current_player == player_computer) {
        uint8_t computer_points = current_player->points[*p_points_index];
        *p_total_computer_roll_value += *p_die_value;
        if ( *p_total_computer_roll_value > MAX_TOTAL_COMPUTER_ROLL_VALUE) {
            printf("max roll value reached! -> %d\n", *p_total_computer_roll_value);
            *p_total_computer_roll_value = 0;
            changeTurn(computer_points, 0);
        }
    }
    else {
        *p_total_computer_roll_value = 0;
    }
}

void checkTurn(uint8_t* p_die_value, uint8_t* p_points_index, uint8_t* p_total_computer_roll_value) {
   if (current_player == player_me) {
        readInput(p_die_value, p_points_index);
    }
    else {
        computerMove(p_die_value, p_points_index, p_total_computer_roll_value); 
    }
}

void showHistory() {
    writeCharToSegment(1, winner==NULL? 'X' : winner->name);
    printf("\n\n\n   HISTORY   \n-------------\n");
    for (uint8_t i = 0; i < history_index; i++) {
        printf("Turn %02d -> %c: %02d point%-2c; Duration: %02d min %02d sec\n", i+1, (*(p_game_history+i))->name, (*(p_game_history+i))->points, (*(p_game_history+i))->points==1? ' ' : 's', (*(p_game_history+i))->duration_in_m, (*(p_game_history+i))->duration_in_s);//print "point"(singular) when 1 point remaining after turn: using %c with the character 's'
    }
    printf("==========\nWinner: %c\n==========\n", winner==NULL? 'X' : winner->name);
}

void freeMem() {
    for (uint8_t i = history_index-1; i <= 0; i--) {
        free(*(p_game_history+i));
    }
    free(p_game_history);
    free(player_computer);
    free(player_me);
    free(current_player);
    free(winner);
}

void playEndTune() {
    if (winner == NULL) {
        playSequenceBackwards(NOTE_DURATION_MS);
    }
    else if (winner == player_computer) {
        playGameLostTune();
    }
    else {
        playGameWonTune();
    }
}

void playGame() {
    uint8_t me_points_index = 0;
    uint8_t computer_points_index = 0;
    uint8_t die_value = 0;
    uint8_t total_computer_roll_value = 0;
    while(!game_over) {
        uint8_t* p_points_index = current_player == player_computer? &computer_points_index : &me_points_index;
        displayValues(p_points_index, die_value);
        checkTurn(&die_value, p_points_index, &total_computer_roll_value);
    } 
    playEndTune();
    showHistory(); 
    freeMem();
}


int main() {
    initDisplay();
    initUSART();
    initADC();
    initTimer();
    enableAllButtons();
    enableAllLeds();
    enableBuzzer();
    while (1) {
    writeCharToSegment(1, 'C');
    }
    /* waitForStart();
    playGame(); */

    return 0;
}