/*
 * Autor: Marco Gruber
 */ 

#ifndef LCD_H_
#define LCD_H_

void initDisplay();
void toggleEnable();
void writeData();
void resetRS();
void setRS();
void write4Bit(char command);
void writeCommand(char command);
void writeCharArray(char *commands, int length);
void clearDisplay();
void setCursorToHome();
void shiftCursorRight();
void setCursor2Line();

#endif /* LCD_H_ */