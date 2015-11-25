/*
 * AntiFanControll.c
 *
 * Created: 07.10.2015 14:16:34
 * Author : Michael M�hlbacher
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "barmeter.h"
#include <avr/interrupt.h>

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1					//RICHTIG??????????

#define TX_BUFF_SIZE 20

#define Q_INCR(x) x = x+1; if (x == TX_BUFF_SIZE) x = 0;
#define UDRIE_on() UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0) | BIT(UDRIE0)
#define UDRIE_off() UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0)

unsigned short actualFanSpeedMeasured = 0;
unsigned short potiValueMeasured = 0;

int current;
int length;
unsigned char* uartString;
int uartStringEmpty = 1;

struct buffer {
	unsigned char volatile head_pos;
	unsigned char volatile empty_pos;
	unsigned char volatile length;
	char volatile buffer[TX_BUFF_SIZE];
};

static struct buffer txBuffer;

static void initBuffer()
{
	txBuffer.head_pos=0;
	txBuffer.empty_pos=0;
	txBuffer.length=0;
}

void usart_send(char ch)
{
	unsigned char len;
	
	//otherwise check queue full
	while ((len=txBuffer.length) == TX_BUFF_SIZE); // block while buffer is full
	
	//add ch to buffer
	txBuffer.buffer[txBuffer.empty_pos] = ch;
	Q_INCR(txBuffer.empty_pos);
	txBuffer.length = len+1;
}

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
	// Set frame format: 8data, 1stop bit
	
	//UBRRH = (uint8_t) (MYUBRR>>8);
	//UBRRL = (uint8_t) MYUBRR;
	
	UBRRL = 51;
	UBRRH = 0;
	
	UCSRB |= (1<<TXEN) | (1<<TXCIE);								// Enable transmitter & interrupt on finish,     (1<<RXEN) for receiver
	//UCSRB |= (1<<UCSZ1) | (1<<UCSZ0);								//UCSZ1,0 set to 1 UCSZ2 set to 0 for 8bit data
	
	UCSRC = (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1);	//Asynchron 8N1
	//UCSRC &= ~(1<<USBS);											//set 1 stop bit    !! strange error when active
	//UCSRC &= ~((1<<UPM1) | (1<<UPM0) | (1<<UMSEL));					//no parity, UMSEL for async operation
}

void initUartString(char* s)
{
	uartString = s;
	length = sizeof(s)/sizeof(s[0]);
	current = 0;
}

void sendUartChar(unsigned char data)
{
	while (!(UCSRA & (1<<UDRE)));		//wartet, bis senden m�glich ist
	
	UDR = data;
}

void sendUartCharString(const char *str)
{
	int a=0;
	while(str[a]!='\0')
		sendUartChar(str[a++]);
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

int main(void)
{
	initBarMeter();
	initDisplay();
	initRPM();
	initTimer1();
	initADC();
	initPWM2();
	initUART();	
	initBuffer();
	
	sei();
	
	//RingBuffer buffer = RingBuffer_create(20);		//ka obs funktioniert
	//writeToDisplay(":D");
	
    for(;;)
    {
		/*if(uartStringEmpty == 1){
			initUartString("lol");
			sendUartChar();
		}*/
		
		setBarMeter(potiValueMeasured);
		
		readADC();
		setCursorToHome();
		writeIntToDisplay((60000000UL/((unsigned long)16*(unsigned long)actualFanSpeedMeasured)));	//Print RPM 
		setCursor2Line();
		writeIntToDisplay((potiValueMeasured*100)/255);	//Print Duty
		setCursor2Line();
		writeIntToDisplay(8 * (unsigned long)actualFanSpeedMeasured);	//Print pulse time
		OCR2 = potiValueMeasured;
		_delay_ms(10);
    }
	
	return 0;
}

ISR(INT0_vect)
{
	actualFanSpeedMeasured = TCNT1;
	TCNT1 = 0x00;
}


ISR(USART_UDRE_vect)	//UART has Transmitted
{
	//character transferred to shift register
	//so UDR is now empty
	//try to get char from tx buffer
	if (txq.length != 0) {
		//get char and send it
		UDR0 = txq.buffer[txq.head_pos];
		Q_INCR(txq.head_pos);
		txq.length = txq.length-1;
		} else {
		UDRIE_off(); // switch off interrupt since no data
	}
}


