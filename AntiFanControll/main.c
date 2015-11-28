 /**
 * @mainpage FanControl MIC WS2015/2016
 *
 * FanControl is a term project within the microcontroller lab course in the 5th semester
 * of the bachelors degree program Information Technology & Systems Management
 * at the Salzburg University of Applied Sciences.
 *
 * @author Stefan Binna
 * @author Michael Mühlbacher
 *
 * @date 25.11.2015
 */

 /**
 * @file main.c
 * @brief Main module
 *
 * This file includes the main function.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "barmeter.h"
#include "CircularBuffer.h"
#include "LCD.h"

void initRPM();
void initTimer1();
void initADC();
void readADC();
void initPWM2();
void initUART();

/**
* @brief Saves the timer value representing the fan speed
*/
unsigned short actualFanSpeedMeasured = 0;

/**
* @brief Saves the ADC value of the potentiometer
*/
unsigned short potiValueMeasured = 0;

/**
* @brief Main entry point of the microcontroller program.
*
* Contains all init functions which get executed only once. Afterwards an infinite loop while(1) will be called for all periodic tasks.
*
* @return int Return code of the main program. Codes other than zero signals an error.
*/
int main(void)
{
	initBarMeter();
	initDisplay();
	initRPM();
	initTimer1();
	initADC();
	initPWM2();
	initCircularBuffer();
	initUSART();		
	
	sei();	//Enable global interrupts
	
	char rpmString[4];
	char timeString[5];
	char dutyString[3];
	
    while(1)
    {
		OCR2 = potiValueMeasured;
		int duty = (potiValueMeasured * 100) / 255;	//Calculate pwm duty
		setBarMeter(potiValueMeasured);
		readADC();
		
		int rpm = 60000000UL / ((unsigned long)16 * (unsigned long)actualFanSpeedMeasured);	//Calculate rpm
		int time = 8 * (unsigned long)actualFanSpeedMeasured;	//Calculate time
		
		sprintf(rpmString,"%4u", rpm);
		
		/* Print to display */
		setCursorToHome();
		writeCharArray(rpmString, 4);
		writeCharArray("rpm ", 5);
		sprintf(timeString,"%5u", time);
		writeCharArray(timeString, 5);
		writeCharArray("us", 2);
		
		setCursor2Line();
		writeCharArray("Duty: ", 6);
		sprintf(dutyString,"%3u", duty);
		writeCharArray(dutyString, 3);
		writeCharArray("%", 1);
		
		/* Write to USART */
		sendUartString("\033", 1);
		sendUartString("\143", 1);
		
		sendUartString(rpmString, 4);
		sendUartString(" RPM | ", 7);
		
		sendUartString(timeString, 5);
		sendUartString(" us\r\n", 5);
		
		sendUartString(dutyString, 3);
		sendUartString("%\r\n",3);
		sendUartString("\r\n",2);
		
		_delay_ms(100);
    }
	
	return 0;
}

/**
* @brief Initialize INT0 for RPM measurements
*
* Configures the external interrupt INT0 to measure the fan speed.
* It triggers on the falling edge.
*
* @return void
*/
void initRPM()
{
	GICR = 1<<INT0;					// Enable INT0
	MCUCR |= 1<<ISC01;				// Trigger INT0 on falling edge
	MCUCR &= ~(1<<ISC00);
}

/**
* @brief Initialize Timer 1 for RPM measurements
*
* Configures the timer 1 for RPM measurements. Normal mode and prescaler 64.
*
* @return void
*/
void initTimer1()
{
	//Normal-Mode
	TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
	TCCR1B &= ~((1 << WGM12) | (1 << WGM13));
	
	//Prescaler 64
	TCCR1B |= ((1 << CS10) | (1 << CS11));
	TCCR1B &= ~((1 << CS12));
	
	TCNT1 = 0;	
}

/**
* @brief Initialize ADC
*
* Initializes the ADC and converts the voltage (from the potentiometer) to a value between 0 and 255.
*
* @return void
*/
void initADC()
{
	DDRA &= ~(1<<PA0);
	ADMUX &= ~((1<<MUX0) | (1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX4));	// ADC input channel set to PA0
	
	ADMUX &=~(1<<REFS1);
	ADMUX |= (1<<REFS0);
	ADCSRA |= (1<<ADEN);

	ADCSRA &= ~(1<<ADPS0);	//Prescaler 64
	ADCSRA |= ((1<<ADPS1)|(1<<ADPS2));

	ADMUX |=(1<<ADLAR);
}

/**
* @brief Read value from ADC
*
* Reads the value from the ADC and saves it to the variable potiValueMeasured.
* The range of the value is between 0 and 255.
*
* @return void
*/
void readADC()
{
	if(!(ADCSRA & (1<<ADSC)))	//still converting
	{
		potiValueMeasured = ADCH;
		ADCSRA |= (1<<ADSC);	//restart
	}
}

/**
* @brief Initialize timer2 for PWM
*
* Configures the Timer 2 to control the fan speed.
*
* @return void
*/
void initPWM2()
{
	DDRD |= 1<<PD7;	//set pin to output

	TCCR2 |= (1<<WGM21)|(1<<WGM20);	//set fastPWM
	OCR2 = 255;	//set compare register
	
	TCCR2 &= ~ ((1<<CS21)|(1<<CS22));	//prescaler = 1;
	TCCR2 |= (1<<CS20);
	TCCR2 |= (1<<COM21);	//Clear OC2 on compare match, set OC2 at BOTTOM,
	TCCR2 &= ~ (1<<COM20);
}

/**
* @brief Initialize USART interface
*
* Initializes Asynchron 8N1 UART interface. 
*
* @return void
*/
void initUSART()	
{
	UBRRH = 0;
	UBRRL = 51;
	
	UCSRB = ((1<<TXEN));
	
	UCSRC = ((1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1));	//Asynchron 8N1
	UCSRB |= (1 << UDRIE);	//no parity, UMSEL for async operation
}

/**
* @brief ISR external interrupt INT0
*
* When the external interrupt INT0 occurs, the value gets saved to the variable actualFanSpeedMeasured.
* Afterwards the Timer1 count register gets set to 0.
*
* @return void
*/
ISR(INT0_vect)
{
	actualFanSpeedMeasured = TCNT1;
	TCNT1 = 0x00;
}

/**
* @brief ISR Data Register Empty Interrupt
*
* When the data register is empty, new data from the buffers gets loaded and sent out the USART interface.
*
* @return void
*/
ISR(USART_UDRE_vect)
{
	if(pRead == pWrite)
	{
		UCSRB &=~ (1 << UDRIE);
		return 1;
	}
	
	UDR = *pRead;
	pRead++;
	if(pRead >= &circularBuffer[CIRCULARBUFFERSIZE])
	{
		pRead = circularBuffer;
	}
}


