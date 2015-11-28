/**
* @file barmeter.c
* @brief Barmeter functions
*
* This module defines the functions for the barmeter.
*
*/

#include <avr/io.h>
#include "LCD.h"

/**
* @brief Set bar meter 
*
* Set the bar meter according to the value, which originally comes from the ADC
*
* void setBarMeter(unsigned int value)
* @param value Value between 0 and 255
* @return void
*/
void setBarMeter(unsigned int value)
{
	PORTA &= ~((1<<PA4)|(1<<PA5)|(1<<PA6)|(1<<PA7));

	if ((value > 51) )	PORTA |= (1<<PA4);
	if ((value > 102) )	PORTA |= (1<<PA4)|(1<<PA5);
	if ((value > 153) )	PORTA |= (1<<PA4)|(1<<PA5)|(1<<PA6);
	if ((value > 204) )	PORTA |= (1<<PA4)|(1<<PA5)|(1<<PA6)|(1<<PA7);
	
}

/**
* @brief Initialize bar meter
*
* Sets the ports to output and to 0.
* @return void
*/
void initBarMeter(void)
{
	DDRA |= (1<<PA4)|(1<<PA5)|(1<<PA6)|(1<<PA7);		//ausgang
	PORTA &= ~(1<<PA4)|(1<<PA5)|(1<<PA6)|(1<<PA7);
}