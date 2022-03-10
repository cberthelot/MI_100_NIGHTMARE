#include "config_bits.h"

#define MASK_AN0   (1<<12)
#define MASK_AN1   (1<<13)
#define MASK_AN8   (1<<8)

#define MASK_LED0  (1<<0)
#define MASK_LED1  (1<<1)
#define MASK_LED2  (1<<2)
#define MASK_LED3  (1<<3)
#define MASK_LED4  (1<<4)
#define MASK_LED5  (1<<5)
#define MASK_LED6  (1<<6)
#define MASK_LED7  (1<<7)
#define FULL_LED   0x00FF



// the wait function
void wait(int l) {
    for (int i=0; i<l; i++) { }
}

// Do I really need to precise that this is the main function ???
void main() {
// Initialization
    char fsm  = 0;                                              // to store the animation step
    char croissant = 1;
    int speed = 5000000;
    char is_push = 1;
    ANSELB = ANSELB & ~(MASK_AN0 | MASK_AN1);                   // inhibit analog functions on B12 and B13
    ANSELB = ANSELB & ~(MASK_AN8);                   // inhibit analog functions on B8
    TRISA  = ~(FULL_LED); // set trisa 0 to 8 in output
    TRISF  = 0x0008;
    TRISB  = 0x0100;
    
// loop
    while(1)
    {
        if ((PORTF & 0x0008)){
            speed = 50000; 
        }
        else{
            speed = 500000;
        }
        wait(speed);          // just wait !
        
        
        //gestion du sens
        if (croissant){
            fsm = (fsm+1)%8;     // next animation step
        }
        else{
            if (fsm == 0){
                fsm = 8;
            }
            fsm = (fsm-1);
        }
        if (((PORTB & 0x0100) == 0x0100) && !is_push){
            is_push = 1;
            croissant = !croissant;
        }
        else if (((PORTB & 0x0100) != 0x0100) && is_push){
            is_push = 0;
        }
        
        PORTA = PORTA & ~(FULL_LED); // on eteint les leds


        switch (fsm) {
            case 0 :
                PORTA = PORTA | MASK_LED0;
                break;
            case 1 :
                PORTA = PORTA | MASK_LED1;
                break;
            case 2 :
                PORTA = PORTA | MASK_LED2;
                break;
            case 3 :
                PORTA = PORTA | MASK_LED3;
                break;
            case 4 :
                PORTA = PORTA | MASK_LED4;
                break;
            case 5 :
                PORTA = PORTA | MASK_LED5;
                break;
            case 6 :
                PORTA = PORTA | MASK_LED6;
                break;
            case 7 :
                PORTA = PORTA | MASK_LED7;
                break;
        }
        
    }
}