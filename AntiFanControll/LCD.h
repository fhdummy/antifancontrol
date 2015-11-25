/*
 * IncFile1.h
 *
 * Created: 02.06.2015 10:28:06
 *  Author: Marco
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

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


#endif /* INCFILE1_H_ */