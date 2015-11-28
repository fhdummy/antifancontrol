/*
 * AntiFanControll.c
 *
 * Created: 07.10.2015 14:16:34
 * Author : Michael Mühlbacher
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "barmeter.h"
#include "CircularBuffer.h"
#include "LCD.h"
#include <avr/interrupt.h>

#define BAUD 9600

int current;
int length;

unsigned short actualFanSpeedMeasured = 0;
unsigned short potiValueMeasured = 0;

void initRPM()
{
	GICR = 1<<INT0;					// Enable INT0
	MCUCR |= 1<<ISC01;				// Trigger INT0 on falling edge
	MCUCR &= ~(1<<ISC00);
}

void initTimer1(void)
{
	//Normal-Mode
	TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
	TCCR1B &= ~((1 << WGM12) | (1 << WGM13));
	
	//Prescaler 64
	TCCR1B |= ((1 << CS10) | (1 << CS11));
	TCCR1B &= ~((1 << CS12));
	
	TCNT1 = 0;
	
	//Interrupt not Enable
	//TIMSK |= (1 << OCIE1A);
	
}

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

void readADC()
{
	if(!(ADCSRA & (1<<ADSC)))	//still converting
	{
		potiValueMeasured = ADCH;
		ADCSRA |= (1<<ADSC);	//restart
	}
}

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

void initUART()		//S.161
{	
	UBRRH = 0;
	UBRRL = 51;
	
	UCSRB = ((1<<TXEN));									//UCSZ1,0 set to 1 UCSZ2 set to 0 for 8bit data
	
	UCSRC = ((1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1));	//Asynchron 8N1
	UCSRB |= (1 << UDRIE);				//no parity, UMSEL for async operation

	initCircularBuffer();
}

int main(void)
{
	initBarMeter();
	initDisplay();
	initRPM();
	initTimer1();
	initADC();
	initPWM2();
	initCircularBuffer();
	initUART();		
	sei();
	
	char rpmString[4];
	char timeString[5];
	char dutyString[3];
	
    while(1)
    {
		OCR2 = potiValueMeasured;
		int duty = (potiValueMeasured * 100) / 255;
		setBarMeter(potiValueMeasured);
		readADC();
		
		int rpm = 60000000UL / ((unsigned long)16 * (unsigned long)actualFanSpeedMeasured);
		int time = 8 * (unsigned long)actualFanSpeedMeasured;
		
		/* Convert integers to strings */
		sprintf(rpmString,"%4u", rpm);
		sprintf(timeString,"%5u", time);
		
		/* Print to display */
		setCursorToHome();
		writeCharArray(rpmString, 4);
		writeCharArray(" RPM", 4);
		
		setCursor2Line();
		writeCharArray("Duty: ", 6);
		sprintf(dutyString,"%3u", duty);
		writeCharArray(dutyString, 3);
		
		
		
		//setCursor2Line();
		//writeIntToDisplay((potiValueMeasured*100)/255);	//Print Duty
		//setCursor2Line();
		//writeIntToDisplay(8 * (unsigned long)actualFanSpeedMeasured);	//Print pulse time
		
		_delay_ms(10);
    }
	
	return 0;
}

ISR(INT0_vect)
{
	actualFanSpeedMeasured = TCNT1;
	TCNT1 = 0x00;
}


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


