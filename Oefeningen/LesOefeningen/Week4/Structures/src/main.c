//#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>

//#include "button.h"
#include "display.h"
#include "led.h"
#include "usart.h"


typedef struct
{
    char* straat;
    uint16_t nummer;
    uint16_t postcode;
}ADRES;

void printAdres(ADRES* ad) {
    printf("%s %d, %d", ad->straat, (*ad).nummer, ad->postcode);
}

/* typedef struct
{
    char naam[10]; //Er wordt altijd 10 plaatsen gereserveerd voor de naam; bij kortere namen is er redundante plaats en bij langere namen is er niet genoeg plaats!! Oplossing: dynamisch! (char* p_naam)
    uint8_t punt;
} STUDENT; */

typedef struct
{
    char* p_naam;
    uint8_t punt;
    ADRES* adres; //mag eventueel een pointer zijn: ADRES* p_adres ipv ADRES adres
} STUDENT;

void printStudent(STUDENT* s) { //call be REFERENCE
    printf("%s: %d/10\n", (*s).p_naam, s->punt); //(*s).blabla = s->blabla
    printAdres(s->adres);
}

/* STUDENT* newStudent(char n[], uint8_t punt) { // char n[] is statisch
    STUDENT* this = calloc(1, sizeof(STUDENT));
    strcpy(this->naam, n);
    (*this).punt = punt;

    return this;
} */
STUDENT* newStudent(char* n, uint8_t punt, ADRES* ad) {
    STUDENT* this = calloc(1, sizeof(STUDENT));
    this->p_naam = malloc(strlen(n)+1);
    strcpy(this->p_naam, n);
    (*this).punt = punt;
    this->adres = ad;
    return this;
}

ADRES* newAdres(char* straatnaam, uint16_t nummer, uint16_t postcode) {
    ADRES* this = calloc(1, sizeof(ADRES));
    this->straat = calloc(strlen(straatnaam)+1, sizeof(char));
    strcpy((*this).straat, straatnaam);
    this->nummer = nummer;
    this->postcode = postcode;
    return this;
}

int main() {
    initDisplay();
    initUSART();
    STUDENT* st;
    ADRES* ad;
    /* 
    st = calloc(1, sizeof(STUDENT));
    strcpy((*st).naam, "Jef");
    (*st).punt = 9; */
    ad = newAdres("Meir", 18, 2000);
    st = newStudent("Marie", 8, ad);
    printStudent(st);
    free(st->p_naam);
    free(st);
    
    return 0;
}