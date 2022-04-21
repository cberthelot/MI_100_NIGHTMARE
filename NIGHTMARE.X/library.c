#include "library.h"
#include <xc.h>
// peripheral clock frequency
#define BASE_TIMER_FREQ 120000000

void wait_timer2_bis() {
    // this function blocks until timer 2 expires
    while ((IFS0 & (1 << 9)) == 0) {
    }
    IFS0 = IFS0 & ~(1 << 9);
}

// Configure tous les registres relatif au Serial avec le baud rate spécifié

void begin(int baudRate) {
    U3STA = (1 << 12) | (1 << 10); //Activation de la reception(bit 12) et de la transmission (bit 10)
    U3MODE = (1 << 15); //Activation de l'UART (bit 15)
    U3BRG = (BASE_TIMER_FREQ / (16 * baudRate)) - 1; //génération du baudRate
    RPC1R = 0x1; //On selectionne le pin de sortie de U3TX (PMODA)
    U3RXR = 0xA; //On selectionne le pin de sortie de U3RX (PMODA)
    TRISC &= ~(1 << 2);
    LATC |= (1 << 2);
    TRISG &= ~(1 << 7);
    LATG |= (1 << 7);
}

// Renvoi 1 si buffer de réception avec au minimum 1 char sinon 0

int RX_available() {
    if (U3STA & 1) {
        return 1;
    }
    return 0;
}

//Renvoi 1 si le buffer TX à encore de la place sinon 0

int TX_available() {
    if (U3STA & (1 << 9)) {
        return 0;
    }
    return 1;
}

//Fonction bloquante jusqu'à la fin de l'envoi du buffer TX

void flush() {
    while (!TX_available()) {
    }
}

//Met dans le buffer de TX ``toPrint`` si il y a de la place renvoi 1 sinon renvoi 0

int print_UART(char toPrint) {
    if (TX_available()) {
        U3TXREG = toPrint; //On remplit le registre U3TXREG
        return 1;
    }
    return 0;
}

//Met dans le buffer de TX 'n' char si il y a de la place renvoi 1 sinon renvoi 0
//TX buff fait 8 deep

int print_UART_n(char *toPrint, int n) {
    int i = 0;
    while (TX_available() && i < n) {
        U3TXREG = toPrint[i]; //On remplit le registre U3TXREG
        i++;
        while (!TX_available()) {
        }
    }
    return 1;
}

//Retourne un char si il y en a un dans le buffer de RX

char read() {
    /*char res;
    if(RX_available){
        res = U3RXREG; // On renvoit ce qui est contenu dans le U3RXREG
    }
    return res;*/
    return U3RXREG;
}


//Configure les leds

void begin_led() {
    TRISA = TRISA & 0xFF00; // set LEDs as output
    LATA &= 0xFF00; // clear all LEDs
}


//Configure les interrupteur

void begin_inter() {
    // SW0/RF3 SW1/RF5 SW2/RF4 SW3/RD15 SW4/RD14 SW5/RB11 SW6/RB10 SW7/RB9
    TRISF |= (1 << 3) | (1 << 5) | (1 << 4); // bit 3, 5, 4 du port F en input
    TRISD |= (1 << 15) | (1 << 14); // bit 15, 14 du port D en input
    TRISB |= (1 << 11) | (1 << 10) | (1 << 9);
    ANSELB &= ~(1 << 11 | 1 << 10 | 1 << 9); //desactivation des fonctions analogique du port B

}

//Lit la valeur de 1 interrupteur

unsigned char read_inter(int number) {
    unsigned char value_switch = 0;
    switch (number) {
        case 0:
            value_switch = PORTFbits.RF3;
            break;
        case 1:
            value_switch = PORTFbits.RF5;
            break;
        case 2:
            value_switch = PORTFbits.RF4;
            break;
        case 3:
            value_switch = PORTDbits.RD15;
            break;
        case 4:
            value_switch = PORTDbits.RD14;
            break;
        case 5:
            value_switch = PORTBbits.RB11;
            break;
        case 6:
            value_switch = PORTBbits.RB10;
            break;
        case 7:
            value_switch = PORTBbits.RB9;
            break;
        default:
            value_switch = 0;
            break;
    }
}



//Lit la valeur de chaque interrupteur

unsigned char read_inters() {
    unsigned char vector = 0;
    for (int i = 0; i < 8; i++) {
        if (read_inter(i))
            vector |= (1 << i);
        else
            vector &= ~(1 << i);
    }
    return vector;
}


//Allume les led en fonction de ce qu'il y a dans buffer

void write_led(unsigned char buffer) {
    for (int i = 0; i < 8; i++) {
        if (buffer & (1 << i))
            LATA |= 1 << i;
        else
            LATA &= ~(1 << i);
    }
}

int begin_BL(int master) {

    U5TXREG = '0';
    char buff[8] = "$$$";
    print_UART_n(buff, 3);
    wait_timer2_bis();

    U5TXREG = '1';
    char buff1[4] = "R,1\r";
    print_UART_n(buff1, 4);
    wait_timer2_bis();
    wait_timer2_bis();
    wait_timer2_bis();


    U5TXREG = '2';
    print_UART_n(buff, 3);
    wait_timer2_bis();


    U5TXREG = '3';
    char buff2[5] = "SA,0\r";
    print_UART_n(buff2, 5);
    wait_timer2_bis();



    U5TXREG = '1';
    print_UART_n(buff1, 4);
    wait_timer2_bis();
    wait_timer2_bis();
    wait_timer2_bis();


    U5TXREG = '2';
    print_UART_n(buff, 3);
    wait_timer2_bis();

    U5TXREG = '4';
    char buff3[2] = "+\r";
    print_UART_n(buff3, 2);
    wait_timer2_bis();

    U5TXREG = '5';
    if (master) {
        while (!TX_available());
        char big_buff[15] = "C,682719F908BD\r";
        print_UART_n(big_buff, 15);
    }
    U5TXREG = '*';
    
    while (RX_available())
    {
        if(U3STA & (1<<1)) U3STA &=U3STA & ~(1<<1);
        U5TXREG = read();
    }
    return 1;
}

int begin_BL_fast(int master) {

    U5TXREG = '5';
    if (master) {
        U5TXREG = '0';
        char buff[8] = "$$$";
        print_UART_n(buff, 3);
        wait_timer2_bis();
        while (!TX_available());
        char big_buff[15] = "C,682719F908BD\r";
        print_UART_n(big_buff, 15);
    }
    U5TXREG = '*';
    
    while (RX_available())
    {
        if(U3STA & (1<<1)) U3STA &=U3STA & ~(1<<1);
        U5TXREG = read();
    }
    return 1;
}