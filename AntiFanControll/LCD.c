/**
* @file LCD.c
* @brief LCD Funktionen
*
* Dieses Modul definiert die Funktionen für das LCD Display.
* Autor: Marco Gruber
*
*/

#include <avr/io.h>
#include <util/delay.h>

/**
* @brief Sendet Enable Impuls ans Display
*/
void toggleEnable() {
	PORTB |= 1<<PB3;
	_delay_us(50);
	PORTB &= ~(1<<PB3);
}

/**
* @brief Setzt RS Pin des Displays
*/
void setRS() {
	PORTB |= 0x04;
}

/**
* @brief Rücksetzt RS Pin des Displays
*/
void resetRS() {
	PORTB &= ~0x04;
}

/**
* @brief Schreibt 4 Datenbit an das Display
*
* void write4Bit(char command)
* @param command 4 Bit an zu sendenden Daten.
*/
void write4Bit(char command) {
	PORTB &= 0x0f;
	PORTB |= (command<<4);
	toggleEnable();
}

/**
* @brief Sendet ein Kommando an das Display
*
* void writeCommand(char command)
* @param command Zu sendendes Kommando in 8 Bit Form.
*/
void writeCommand(char command) {
	resetRS();
	write4Bit((command&0xf0)>>4);
	write4Bit(command&0x0f);
}

/**
* @brief Initialisiert das Display
*
*/
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
	
	writeCommand(0x0E);
	
	clearDisplay();
}

/**
* @brief Setzt den Display Curser an die erste Spalte, erste Zeile
*
*/
void setCursorToHome() {
	writeCommand(0x02);
	_delay_ms(2);
}

/**
* @brief Setzt den Display Curser an die erste Spalte, zweite Zeile
*
*/
void setCursor2Line() {
	writeCommand(0xc0);
	_delay_ms(1);
}

/**
* @brief Setzt den Display Curser um eins nach Rechts
*
*/
void shiftCursorRight() {
	writeCommand(0x14);
	
}

/**
* @brief Sendet ein Zeichen an das Display
*
* void writeData(char data)
* @param data Zu sendendes Zeichen
*/
void writeData(char data) {
	setRS();
	write4Bit((data&0xf0)>>4);
	write4Bit(data&0x0f);
	resetRS();
}

/**
* @brief Löscht den Displayinhalt
*
*
*/
void clearDisplay() {
	writeCommand(0x0);
	_delay_ms(3);
	writeCommand(0x1);
	_delay_ms(3);
}

/**
* @brief Sendet ein Character Array an das Display
*
* writeCharArray(char *commands,int length)
* @param commands Zeiger auf das Character Array
* @param length Länge des Character Arrays
*/
void writeCharArray(char *commands,int length) {
	int i;
	for(i=0;i < length;i++) {
		writeData(commands[i]);
	}
	_delay_us(100);
}