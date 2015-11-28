#include <avr/io.h>
#include <util/delay.h>

/* Copyright by Marco Gruber */

void toggleEnable() {
	PORTB |= 1<<PB3;
	_delay_us(50);
	PORTB &= ~(1<<PB3);
}

void setRS() {
	PORTB |= 0x04;
}

void resetRS() {
	PORTB &= ~0x04;
}

void write4Bit(char command) {
	PORTB &= 0x0f;
	PORTB |= (command<<4);
	toggleEnable();
}

void writeCommand(char command) {
	resetRS();
	write4Bit((command&0xf0)>>4);
	write4Bit(command&0x0f);
}

void initDisplay() {
	DDRB |= 0xfc;
	resetRS();
	
	PORTB |= 0x30;
	PORTB &= 0x3f;
	_delay_ms(15);
	toggleEnable();
	_delay_ms(5);
	toggleEnable();
	_delay_us(200);
	toggleEnable();

	PORTB |= 0x20;
	PORTB &= 0x2f;
	toggleEnable();
	
	_delay_ms(3);
	
	writeCommand(0x2C);
	/*_delay_ms(3);
	writeCommand(0xC);
	_delay_ms(3);*/
	
	writeCommand(0x0E);
	/*_delay_ms(3);
	writeCommand(0xe);
	_delay_ms(3);*/
	
	writeCommand(0x0C);
	
	clearDisplay();
}

void setCursorToHome() {
	writeCommand(0x02);
	_delay_ms(2);
}

void setCursor2Line() {
	writeCommand(0xc0);
	_delay_ms(1);
}

void shiftCursorRight() {
	writeCommand(0x14);
	
}

void writeData(char command) {
	setRS();
	write4Bit((command&0xf0)>>4);
	write4Bit(command&0x0f);
	resetRS();
}

void clearDisplay() {
	writeCommand(0x0);
	_delay_ms(3);
	writeCommand(0x1);
	_delay_ms(3);
}
void writeCharArray(char *commands,int length) {
	int i;
	for(i=0;i < length;i++) {
		writeData(commands[i]);	
	}
}

void writeToDisplay(char* s)
{
	writeCharArray(s, sizeof(s)/sizeof(s[0]));
}

void writeIntToDisplay(int value)
{
	char tempString[15];
	sprintf(tempString,"%15u", value);	

	writeCharArray(tempString, sizeof(tempString)/sizeof(tempString[0]));
}

