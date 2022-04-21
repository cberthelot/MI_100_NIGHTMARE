#include "config_bits.h"
#include "library.h"
#include "minimal_lcd.h"
#include <stdlib.h>

// peripheral clock frequency
#define BASE_TIMER_FREQ 120000000

// masks for 7 segs displays
#define MASK_SEG_A (1<<12)
#define MASK_SEG_B (1<<14)
#define MASK_SEG_C (1<< 6)
#define MASK_SEG_D (1<<13)
#define MASK_SEG_E (1<<15)
#define MASK_SEG_F (1<< 7)
#define MASK_SEG_G (1<<13)
#define MASK_POINT (1<<14)
#define MASK_AN0   (1<<12)
#define MASK_AN1   (1<<13)
#define MASK_AN2   (1<< 9)
#define MASK_AN3   (1<<10)
#define MASK_LED0  (1<< 0)

// indexed access to mask and register of 7 seg display anodes
const uint32_t SEG7_AFF_MASK[4] = {MASK_AN0, MASK_AN1, MASK_AN2, MASK_AN3};
volatile uint32_t *SEG7_AFF_LAT[4] = {&LATB, &LATB, &LATA, &LATA};
int nb_trame = 0;

// indexed access to mask and register of 7 seg display segemnts
// 0:seg a, 1:seg b, .... 6:seg g, 7:point
const uint32_t SEG7_SEG_MASK[8] = {MASK_SEG_A, MASK_SEG_B, MASK_SEG_C, MASK_SEG_D, MASK_SEG_E, MASK_SEG_F, MASK_SEG_G, MASK_POINT};
volatile uint32_t *SEG7_SEG_LAT[8] = {&LATG, &LATA, &LATD, &LATG, &LATG, &LATD, &LATD, &LATG};

// digit representation expressed in 7 seg : bit 0 : seg a, ... , bit 6 : seg g, bit 7 : point
const char digit_7seg[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

void wait_timer1() {
    // this function blocks until timer 1 expires
    while ((IFS0 & (1 << 4)) == 0) {
    }
    IFS0 = IFS0 & ~(1 << 4);
}

void wait_timer2() {
    // this function blocks until timer 2 expires
    while ((IFS0 & (1 << 9)) == 0) {
    }
    IFS0 = IFS0 & ~(1 << 9);
}

void wait_timer3() {
    // this function blocks until timer 3 expires
    while ((IFS0 & (1 << 14)) == 0) {
    }
    IFS0 = IFS0 & ~(1 << 14);
}

void sleep() {
    // go to sleep 
    __asm__ volatile("wait");

}

void init_7seg(void) {
    ANSELB = ANSELB & ~(MASK_AN0 | MASK_AN1); // inhibit analog functions on B12 and B13
    TRISB = TRISB & ~(MASK_AN0 | MASK_AN1); // set output mode for B12, B13
    TRISA = TRISA & ~(MASK_AN2 | MASK_AN3 | MASK_SEG_B); // set output mode for A9,  A10, A14
    TRISG = TRISG & ~(MASK_SEG_A | MASK_SEG_D | MASK_SEG_E | MASK_POINT); // set output mode for G12, G13, G14, G15
    TRISD = TRISD & ~(MASK_SEG_C | MASK_SEG_F | MASK_SEG_G); // set output mode for D6,  D7,  D13

    LATB = LATB | MASK_AN0 | MASK_AN1;
    LATA = LATA | MASK_AN2 | MASK_AN3;

    LATG = LATG | MASK_SEG_A; // set G12 : SEG A = 1 (off)
    LATA = LATA | MASK_SEG_B; // set A14 : SEG B = 1 (off)
    LATD = LATD | MASK_SEG_C; // set D6  : SEG C = 1 (off)
    LATG = LATG | MASK_SEG_D; // set G13 : SEG D = 1 (off)
    LATG = LATG | MASK_SEG_E; // set G15 : SEG E = 1 (off)
    LATD = LATD | MASK_SEG_F; // set D7  : SEG F = 1 (off)
    LATD = LATD | MASK_SEG_G; // set D13 : SEG G = 1 (off)
    LATG = LATG | MASK_POINT; // set G14 : POINT = 1 (off)

}
char displayed_7seg = 0; // to memorize which display is considered
char seg_map[4] = {0, 0, 0, 0}; // to memorize segments of each display
char precounter = 10; // counter predivision
int counter = 0; // the counter to display
int counter_cpy; // temporary copy of the counter
char i;


int clignoter = 0;
int timer3_divider = 5;

void __attribute__((interrupt(ipl7soft), vector(12))) fonction_it(void) {

    IFS0 = IFS0 & ~(1 << 14);
    if (--timer3_divider == 0) {
        // events run at 10Hz, so we have to divide by 10.
        timer3_divider = 1;
        clignoter = 1;
    }
    //PORTA = ~PORTA;
}

void __attribute__((interrupt(ipl7soft), vector(4))) fonction_it_bis(void) {

    // reset timer1 flag
    IFS0 = IFS0 & ~(1 << 4);

    // clear all 7seg display
    LATA |= MASK_AN2 | MASK_AN3 | MASK_SEG_B;
    LATB |= MASK_AN0 | MASK_AN1;
    LATD |= MASK_SEG_C | MASK_SEG_F | MASK_SEG_G;
    LATG |= MASK_SEG_A | MASK_SEG_D | MASK_SEG_E | MASK_POINT;

    // update the display to be powered
    displayed_7seg = (displayed_7seg + 1) & 0x3;

    // light on selected display == 0
    *(SEG7_AFF_LAT[displayed_7seg]) &= ~SEG7_AFF_MASK[displayed_7seg];

    // light on selected segments
    for (i = 0; i < 8; i++) {
        if (seg_map[displayed_7seg] >> i & 1) {
            *(SEG7_SEG_LAT[i]) &= ~SEG7_SEG_MASK[i];
        }
    }
}
int cpt=0;
void __attribute__((interrupt(ipl7soft), vector(8))) fonction_it_tris(void) {


    // reset timer2 flag
    IFS0 = IFS0 & ~(1 << 9);

    // increment counter modulo 10000.
    // FIXME : is testing faster than computing a modulo ???
    if (counter >= 10000) counter = 0;

    // updates seg_map according to counter

    if (cpt > 31) {
		LCD_Clear();
        cpt = 0;
        }
        (cpt < 16) ? LCD_Set_Cursor_Pos(0, cpt) : LCD_Set_Cursor_Pos(1, cpt - 16);
        cpt++;
        
    LCD_Write_HEX(67)
    counter_cpy = nb_trame;

    for (i = 0; i < 4; i++) {
        seg_map[i] = digit_7seg[counter_cpy % 10];
        counter_cpy /= 10;
    }

}
char RX_buf;
char cmd[32];
int fin_cmd = 0;
int cpt;

void __attribute__((interrupt(ipl7soft), vector(40))) fonction_U5R(void) {

    if (IFS2 && (1 << 5)) {
        while (U5STA & 1) {
            counter++;
            cpt++;
            RX_buf = U5RXREG;
            if (cpt > 31) {
                LCD_Clear();
                cpt = 0;
            }
            (cpt < 16) ? LCD_Set_Cursor_Pos(0, cpt) : LCD_Set_Cursor_Pos(1, cpt - 16);
            LCD_Write_Char(RX_buf);
            cpt++;
            while (U5STA & (1 << 9)) {
            }
            print_UART(RX_buf);
        }

        IFS2 = IFS2 & ~(1 << 5);
    }
}

int foo() {
    print_UART(15);
    return 0;
}

typedef struct test {
    int (*send)(void);
} test;




void main() {
    // variables
    // loop counter    

    // Initialization
    init_7seg();

    TRISA = TRISA & 0xFF00; // set LEDs as output
    LATA &= 0xFF00; // clear all LEDs

    // Timer 1 initialization @ 400 Hz
    T1CON = 0x8010; // set prescaler : /8
    PR1 = BASE_TIMER_FREQ / (8 * 400);

    // Timer 2 initialization @ 10 Hz
    T2CON = 0x8070; // set prescaler : /256
    PR2 = BASE_TIMER_FREQ / (256 * 10);

    // Timer 3 initialization @ 8 Hz
    T3CON = 0x8070; // set prescaler : /256
    PR3 = BASE_TIMER_FREQ / (256 * 8);

    //INTCON config
    INTCON = INTCON | 1 << 12;
    IEC0 = IEC0 | (1 << 14 | 1 << 4 | 1 << 9);
    IPC3 = IPC3 | 0x1F;
    IPC1 = IPC1 | 0x1F;
    IPC2 = IPC2 | 0x1F;
    IEC2 = IEC2 | 1 << 5; // interuption U5 receive
    IEC2 = IEC2 & ~(1 << 4 | 1 << 6);
    IPC10 = IPC10 | 0x1F;

    // loop

    //UART
    U5STA = (1 << 12) | (1 << 10);
    U5MODE = (1 << 15);
    U5BRG = (120000000 / (16 * 115200)) - 1;
    
    //LCD
    LCD_Init(1, 1);
    LCD_Clear();
    
    RPF12R = 0x4;
    U5RXR = 0x9;
    __asm__("ei");
    //LCD
    LCD_Init(1, 1);
    LCD_Clear();
    int cpt = 0;

    for (int i = 0; i < 8; i++)
        U5TXREG = '-';
#define chaine "Bonjour le monde\n\r"
    //char *TX_buf = chaine;
    begin_led();
    begin_inter();
    begin(115200);

    U5TXREG = 'A';
    wait_timer2();
    U5TXREG = 'A';
    U5TXREG = '\n';
    U5TXREG = '\r';


    begin_BL_fast(1);// 1 = master

    char buffer=0;

    struct test letest;
    letest.send=&foo;;
    while (1) {
        if(U3STA & (1<<1)) U3STA &=U3STA & ~(1<<1);
        if (nb_trame == 1000)
            nb_trame = 0;
        if (RX_available()) {
            
            buffer = read();
            nb_trame++;
            U5TXREG = 'a'+buffer;
            U5TXREG='|';
            write_led(buffer);
            
        }
        
        if (clignoter) {
            clignoter=0;
            U5TXREG='@';
            letest.send();
            //print_UART(read_inters());
        }



    }
}
