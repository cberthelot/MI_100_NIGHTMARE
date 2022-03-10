#ifndef _MINIMAL_LCD_H
#define _MINIMAL_LCD_H

#define LCD_HOME  LCD_w_Command(0x02)

void LCD_Init(unsigned char CursorOn, unsigned char Blinking);
void LCD_Set_Cursor_Pos(unsigned char line, unsigned char col);
void LCD_Write_Char(unsigned char data);
void LCD_Write_HEX(unsigned char data);
void LCD_Write_String(char *str);
void LCD_Clear();

// private
void LCD_w_Command(unsigned char data);
void DelayUs(unsigned int us);

#endif // _MINIMAL_LCD_H
