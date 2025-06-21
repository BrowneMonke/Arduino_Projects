#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#include "button.h"
#include "display.h"
#include "led.h"
#include "usart.h"

#define MAX 7

int bepaalPos(char* inputString, char teZoekenChar) {
    for (int i = 0; inputString[i] != '\0'; i++) {
        if (*inputString == teZoekenChar) {
            return i;
        }
        inputString++;
    }
    return -1;
}

void drukEersteLetters(char namen[][10]) {
    for (int i = 0; i < MAX; i++) {
        printf("%c\n", namen[i][0]);
        // namen++;
    }
}

int main() {
      initUSART();
      /*
      printf("sizeof int: %d double: %d char: %d\n", sizeof(int),
      sizeof(double),sizeof(char));

      int getallen[5]={4,1,6,2,3};
      //int pos=bepaalPos(getallen,5,6);
      int* pgetallen = calloc(5, sizeof(int));

      for (uint8_t i = 0; i < 5; i++) {
        pgetallen[i] = i*3;
        printf("%d\n", i*3);
      }
      for (uint8_t i = 0; i < 5; i++) {
        printf("\n%d: %d\n", i, *(pgetallen+i));
      }

      free(pgetallen); */

    // Opdracht:
    // 20 doubles 1 rij tafel van 3.1 en een rij tafel van 2.1
    // gebruik geen []
    // vul en print in verschillende lussen
                        //code getest met int en het werkt!
    double** ppdecimals = calloc(2, sizeof(double*));
    
    *ppdecimals = calloc(10, sizeof(double));
    *(ppdecimals+1) = calloc(10, sizeof(double));
    
    for (uint8_t i = 0; i < 10; i++) {
        *(*(ppdecimals) + i) = 3.1 * i;
        *((*(ppdecimals + 1)) + i) = 2.1 * i;
    }
    /*
      for (uint8_t i = 0; i < 10; i++) {
        *((*(decimals+1)) + i) = 2.1*i;
      } */

    for (uint8_t i = 0; i < 10; i++) {
        printf("\n%d", *(*(ppdecimals) + i));
        printf("\t\t%d", *((*(ppdecimals + 1)) + i));
    }

    free(ppdecimals);

    /*
      printf("\npositie:%d\n",pos);
      char naam[]="Josephine-Charlotte";
      pos=bepaalPos(naam,'i');

      printf("\npositie 6 (2):%d\n",pos);

      char namen[MAX][10] = {"Hans", "Anniek", "Wim", "Wouter", "Dominique",
      "Maarten", "Mark"}; drukEersteLetters(namen); drukLaatsteLetters(namen);
      char* kleinste = zoekKleinsteNaam(namen);
      printf("Kleinste = %s\n", kleinste); */

    /* char* buff = malloc(100); //To check at what point the RAM is completely used up
    if (buff == NULL) {
        printf("Geen plaats\n");
    }
    else {
        printf("buff: %p\n", buff);
    } */

    return 0;
}