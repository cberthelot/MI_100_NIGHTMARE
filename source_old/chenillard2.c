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
    OC3CON = 0x8006;
    OC3R=500;
    OC3RS=0;
    OC5CON = 0x8006;
    OC5R=500;
    OC5RS=0;
    OC4CON = 0x8006;
    OC4R=500;//rapport cyclique
    OC4RS=0;
    RPD2R =0b1011;
    RPD12R =0b1011;
    RPD3R =0b1011;
    
    TMR2 = 0;
    //PR2 = 46875; pour 1hz
    PR2 = 10000;
    
    
    PORTA = PORTA | MASK_LED0;
    ANSELA = ANSELA & ~MASK_LED0;
    int counter = 0;

    // loop
    while (1) {
        if (IFS0 & (1<<9)) {
            counter++;
            //PORTA = PORTA | MASK_LED0;
            IFS0=IFS0 & ~(1<<9);
            OC3RS+=15;
            OC5RS+=8;
            OC4RS+=30;
        }
        if(OC3RS >600) OC3RS=0;
        if(OC5RS >600) OC5RS=0;
        if(OC4RS >600) OC4RS=0;
        
        if (counter > 5 ) {
            PORTA = PORTA | MASK_LED0;
          
        } else {
            PORTA = PORTA & ~MASK_LED0;
        }
        if (counter == 10)counter = 0;
    }
}