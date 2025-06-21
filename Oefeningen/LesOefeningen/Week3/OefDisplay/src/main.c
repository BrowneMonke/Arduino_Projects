#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "led.h"
#include "button.h"
#include "display.h"

int main() {
    initDisplay();
    initUSART();

    while(1) {
        //1234567890 laten "scrollen" over de display
        /*         
        for (uint8_t i = 0; i < 10; i++) {
            uint8_t d = i;
            uint8_t h = (i+1) % 10;
            uint8_t t = (i+2) % 10;
            uint8_t e = (i+3) % 10;
            
            short getal = d*1000 + h*100 + t*10 + e;
            writeNumberAndWait(getal, 1000);

        } 
        */
       //testen welke led welk deel
        writeCharToSegment(0, 'A');
        writeCharToSegment(1, 'B');
        writeCharToSegment(2, 'C');
        writeCharToSegment(3, 'D');
    }

    return 0;
}