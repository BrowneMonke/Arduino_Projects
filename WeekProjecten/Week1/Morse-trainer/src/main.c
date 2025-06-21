#include <avr/io.h>
#include <util/delay.h>
#include <time.h>
#include <string.h>

#include <stdlib.h>

#include "led.h"
#include "usart.h"

#define SHORT_DUR 200
#define LONG_DUR 1000
#define PAUSE_DUR 500

const char morsecode[36][6] = 
    {
        ".-",       //A
        "-...",     //B
        "-.-.",     //C
        "-..",      //D
        ".",        //E
        "..-.",     //F
        "--.",      //G
        "....",     //H
        "..",       //I
        ".---",     //J
        "-.-",      //K
        ".-..",     //L
        "--",       //M
        "-.",       //N
        "---",      //O
        ".--.",     //P
        "--.-",     //Q
        ".-.",      //R
        "...",      //S
        "-",        //T
        "..-",      //U
        "...-",     //V
        ".--",      //W
        "-..-",     //X
        "-.--",     //Y
        "--..",     //Z
        "-----",    //0
        ".----",    //1
        "..---",    //2
        "...--",    //3
        "....-",    //4
        ".....",    //5
        "-....",    //6
        "--...",    //7
        "---..",    //8
        "----.",    //9
    };

const char characters[36] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

 

void encode(char morse_sign){
    lightUpAllLeds();
    if (morse_sign == '.') {
        _delay_ms(SHORT_DUR);
    }
    else {
        _delay_ms(LONG_DUR);
    }
    lightDownAllLeds();
    _delay_ms(PAUSE_DUR);
}


void encodeWord(char word[]){
    short word_index = 0;
    while(word_index < strlen(word)){
        short j = 0;
        while(word[word_index] != characters[j]){
            j++;
        }
        
        //show morse code on shield 
        countdownLights();
        for (short sign_index = 0; sign_index < strlen(morsecode[j]); sign_index++) {
            encode(morsecode[j][sign_index]);
        }
        printf("%c\n", characters[j]);

        word_index++;
    }
}



int main(){
    //initDisplay();
    initUSART();
    
    enableAllLeds();

    time_t t;
    srand((unsigned) time(&t));

    printf("\n\n\n");
    for (short round_counter = 0; round_counter < 5; round_counter++) {
        lightUpAllLeds();
        countdownLights();

        short char_index = rand() % 36;
        for (short sign_index = 0; sign_index < strlen(morsecode[char_index]); sign_index++) {
            encode(morsecode[char_index][sign_index]);
        }

        _delay_ms(500);

        printf("Round: %d; Letter: %c (%s)\n", round_counter + 1, characters[char_index], morsecode[char_index]);
    }

    //encodeWord("DAG");

    ledDance();

    return 0;
}