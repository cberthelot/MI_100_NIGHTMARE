


/* ************************************************************************** */
#include <xc.h>
#include "minimal_lcd.h"
/* ************************************************************************** */

// these definition makes it possible to access the lower byte of LATE and TRISE
// as a single byte to improve performance
#define LCD_DATA            (*((unsigned char *)(&LATE)))
#define LCD_TRIS            (*((unsigned char *)(&TRISE)))
#define LCD_MASK            ((unsigned char)0xFF)
#define LCD_DATA_AS_INPUT   LCD_TRIS = 0xFF;
#define LCD_DATA_AS_OUTPUT  LCD_TRIS = 0;

#define EN_REG              LATD
#define EN_MASK             (1<<4)
#define CLEAR_EN            LATDCLR = EN_MASK
#define SET_EN              LATDSET = EN_MASK

#define RW_REG      LATD
#define RW_MASK     (1<<5)
#define CLEAR_RW    LATDCLR = RW_MASK
#define SET_RW      LATDSET = RW_MASK

#define RS_REG      LATB
#define RS_MASK     (1<<15)
#define CLEAR_RS    LATBCLR = RS_MASK
#define SET_RS      LATBSET = RS_MASK




// This function wait for (at least) a given time expressed in microseconds
// the actual time will always be a bit higher.
void DelayUs(unsigned int us) {
    if (us<540) {
        IFS0  = IFS0 & ~(1 << 24);
        PR5   = us*120;
        T5CON = 0x8000;
        while((IFS0 & 1<<24)==0) {}
        T5CON = 0;
        return;
    }
    if (us<8500) {
        IFS0  = IFS0 & ~(1 << 24);
        PR5   = (us*120+15)/16;
        T5CON = 0x8040;
        while((IFS0 & 1<<24)==0) {}
        T5CON = 0;
        return;
    }
    IFS0  = IFS0 & ~(1 << 24);
    PR5   = (us*120+255)/256;
    T5CON = 0x8070;
    while((IFS0 & 1<<24)==0) {}
    T5CON = 0;
}  

void LCD_Init(unsigned char CursorOn, unsigned char Blinking) {

    unsigned char opt = 0;
    if (CursorOn) opt += 2;
    if (Blinking) opt += 1;
    

    TRISB  &= ~(1<<15);  // RB15 as output (RS))
    ANSELB &= ~(1<<15);  // disable analog functionality on RB15
    TRISD  &= ~(3<< 4);  // RD4 and RD5 as output (EN and RW)
    LCD_DATA_AS_OUTPUT;
    ANSELE &= 0xFF00;    // disable Analog on E0 - E7

    CLEAR_RW;
    
	DelayUs(40000);             // wait for 40 ms
	LCD_w_Command(0x3C);        // Function Set (1st call)
	DelayUs(100);               // wait for 100 us
	LCD_w_Command(0x3C);        // Function Set (2nd call))
	DelayUs(100);               // wait for 100 us
	LCD_w_Command(0x0C | opt);  // Display Set
	DelayUs(100);               // wait for 100 us
    LCD_w_Command(0x01);        // LCD Display Clear
	DelayUs(1520);              // Wait 1.52 ms
	LCD_w_Command(0x06);        // Entry mode set
	DelayUs(1520);              // Wait 1.52 ms
}



void LCD_w_Command(unsigned char data) {
    DelayUs(100);
    CLEAR_RS;
    LCD_DATA = data;
    DelayUs(100);  
    SET_EN;
    DelayUs(50);
    CLEAR_EN;
    DelayUs(50);
}


void LCD_Write_Char(unsigned char data) {
    DelayUs(100);  
    SET_RS;
    LCD_DATA = data;
    DelayUs(100);   
    SET_EN;
    DelayUs(50);
    CLEAR_EN;
    DelayUs(50);    
}

void LCD_Write_HEX(unsigned char data) {
    unsigned char nibble = (data >> 4) + '0';
    unsigned char cnt;
    for (cnt = 0; cnt <2; cnt ++) {
        if (nibble > '9') {
            nibble += 'A' - 10 - '0';
        }
        LCD_Write_Char(nibble);
        nibble = (data & 0x0F) + '0';
    }
}


void LCD_Set_Cursor_Pos(unsigned char line, unsigned char col) {
    if (line)
        col += 0x40;
    LCD_w_Command(0x80 | col);
}

void LCD_Write_String(char *str)
{
	while(*str )	{
		LCD_Write_Char(*str);
		str++;
	}
}

void LCD_Clear() {
    LCD_w_Command(0x01);        // LCD Display Clear
	DelayUs(1520);              // Wait 1.52 ms

}