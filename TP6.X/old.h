#include "config_bits.h"

#define MASK_AN0   (1<<12)
#define MASK_AN1   (1<<13)
#define MASK_AN8   (1<<8)

#define MASK_LED0  (1<<0)




// Do I really need to precise that this is the main function ???
void wait(int l) {
    for (int i=0; i<l; i++) { }
}


void main() {
    // Initialization
    TRISA &= ~MASK_LED0; // set trisa 0 to 8 in output
    T2CON = 0x8070;
    //OC1CON = 0x8006;
    TMR2 = 0;
    PR2 = 46875;
    PORTA = PORTA | MASK_LED0;
    ANSELA = ANSELA & ~MASK_LED0;
    int counter = 0;

    // loop
    while (1) {
        if (IFS0 & (1<<9)) {
            counter++;
            //PORTA = PORTA | MASK_LED0;
            IFS0=IFS0 & ~(1<<9);
        }
         
        
        if (counter > 5 ) {
            PORTA = PORTA | MASK_LED0;
        } else {
            PORTA = PORTA & ~MASK_LED0;
        }
        if (counter == 10)counter = 0;
    }
}