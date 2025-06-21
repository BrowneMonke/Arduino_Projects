#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "led.h"
#include "button.h"

ISR(PCINT1_vect) {
  if (bit_is_set(PINC, PC1)) {
    lightUpAllLeds();
    _delay_ms(2000);
    lightDownAllLeds();
  }
}

//#define MAX 5 //part of Pointers vs arrays

///* button + LED
//opgave 1
void testButton(){
  PORTC = 1 << PC1; //pull up resistor aanzetten

  DDRC &= ~(1<<PC1); //knop 1 Data Direction Register op 0 voor PC1(knop 1) en de rest niet beïnvloeden
  printf("DDRC: ");
  printBinaryByte(DDRC);
  printf("\n");

  while(1){
    PINC & (1 << PC1)? printf("Knop 1 is NIET ingedrukt.") : printf("Knop 1 is ingedrukt.");
    printf("\n");
    _delay_ms(1000);
  }
}

//opgave 2
void buttonEnLed(){
  PORTC = 1 << PC1;
  DDRC &= ~(1<<PC1);
  enableLeds(0b00001100);
  lightUpLed(3);

  while(1){
    if (!(PINC & (1 << PC1))) {
      _delay_us(500);
      if (!(PINC & (1 << PC1))) {
        PORTB ^= 0b00001100;
      }
    }
  }

}

int main(){
  initUSART();

  cli();

  buttonEnLed();

  return 0;
} //*/


/* Pointer function
void incrementChar(char* pchar) {
  (*pchar)++;
}

void increment(int* p){
    printf("Incrementing a (%d)\n",*p);
    (*p)++;
}
int main(){
  initUSART();
  
  int a = 10;
  int* pa;
  pa = &a;

  char character = 'a';

  printf("\n pointer %p \n", pa);
  printf("\n pointer %p \n", &a);
  printf("vooraf a: %c", character);
  
  increment(pa);
  incrementChar(&character);

  printf("Value of a after increment: %d\n",*pa);
  printf("hopelijk b: %c", character);
  
  return 0;
} */


/* Canvas: Pointer explanation

void increment_byref(int* a){
    printf("Incrementing a (%d)\n", *a);
    (*a)++;
}

void increment_byval(int a){
    printf("Incrementing a (%d)\n",a);
    a++;
}

void array_as_parameter(int* p){
  //hoe groot is de array in bytes?
  printf("sizeof array as parameter:%d\n",sizeof(p));

}

int main() {
  int a = 10;
  int* b = &a;
  int c[] = {1,2,3};
  int d[] = {1,2,3,4,5,6};

  printf("Value of a: %d\n",a); 
  //hoeveel bytes neemt een int in beslag?
  printf("sizeof a:%d\n",sizeof(a));
  //op welk adres is a opgeslagen (hexadecimaal)? 
  //(Je kan %d gebruiken als je het adres decimaal wil zien, negeer dan de warning...)
  printf("Value of &a: %p\n",&a);
  //b bevat het adres van a
  printf("Value of b: %p\n",b);
  //hoe groot is b (dus hoe groot is een int pointer)?
  printf("sizeof b:%d\n",sizeof(b));
  //en wat is het adres van b zelf?
  printf("Value of &b: %p\n",&b);
  //een array is eigenlijk een pointer naar het eerste element...
  printf("Value of c: %p\n",c);
  //een array is dus hetzelfde als het adres van het eerste element...
  printf("Value of &c[0]: %p\n",&c[0]);
  //hoe groot is de array in bytes?
  printf("sizeof c:%d\n",sizeof(c));
  //geef array als parameter mee
  array_as_parameter(c);
  printf("Value of d: %p\n",d);
  printf("sizeof d:%d\n",sizeof(d));
  increment_byval(a);
  printf("Value of a after by val increment: %d\n",a);
  increment_byref(&a);
  printf("Value of a after by ref increment: %d\n",a);

  return 0;  
}//*/


/* Pointers (dynamic) vs arrays (static)

//Oef 2
int getallenZoeker(int get[], int aantal, int teZoeken) {
  int *ptab;
  ptab = get;
  for (int i = 0; i < aantal; i++) {
    if (*ptab == teZoeken) {
      return i;
    }
    ptab++;
  }

   
  //for (int i = 0; i < aantal; i++) {
    //if (get[i] == teZoeken) {
      //return i;
    //}
  //} 
  return -1;
}

//Oef 3
int bepaalPos(char *inputString, char teZoekenChar) {
  for (int i = 0; inputString[i] != '\0'; i++) {
    if (*inputString == teZoekenChar) {
      return i;
    }
    inputString++;
     
    //if (inputString[i] == teZoekenChar) {
      //return i;
    //}
  }
  return -1;
}

//Oef 4
void drukEersteLetters(char namen[][10]) {
  for (int i = 0; i < MAX; i++) {
    printf("%c\n", namen[i][0]);
    //namen++;
  }
}

int main() {
  initUSART();

  //oef 1
  printf(" int: %d \n double: %d \n char: %d\n\n", sizeof(int), sizeof(double), sizeof(char));
  
  //oef 2
  int getallen[5] = {4, 12, 2, 5, 6};
  int pos = getallenZoeker(getallen, 5, 6);
  printf("pos: %d", pos);
  
  //oef 3 
  char naam[] = "Josephine-Charlotte";
  int pos = bepaalPos(naam, 'i');
  printf("\npositie: %d", pos);

  //oef 4
  char namen[MAX][10] = {"Jules", "Assia", "Akshat", "Jonas", "Reza"};
  drukEersteLetters(namen);

} */


/*  INTERRUPT
int main() {
  initUSART();
  enableAllLeds();
  enableButton(0);
  //pull up aanzetten
  PORTC = 1 << PC1;
  PCICR |= _BV(PCIE1); // PORTC interrupts mogelijk
  PCMSK1 |= 1 << PC1; // Pin Change Mask Register
  sei(); // Set Enable Interrupts
  //cli(); // Clear Interrupts <=> sei()

  while(1) {
    lightDownLed(0);
    lightUpLed(1);
    _delay_ms(1000);
    lightDownLed(1);
    lightUpLed(0);
    _delay_ms(1000);
  }

  return 0;
}
 //*/