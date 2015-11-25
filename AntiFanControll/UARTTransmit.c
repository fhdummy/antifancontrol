/*
 * CFile1.c
 *
 * Created: 25.11.2015 15:59:50
 *  Author: Michael Mühlbacher
 */ 

int _writeCharIfPossible(unsigned char* c, int* length, int* current)
{
	if ((UCSRA & (1<<UDRE)))
	{
		UDR = c;
		*current++;
		if(*current == *length)
		{
			return 1;
		}
	}
	return 0;
}


/* puts ist unabhaengig vom Controllertyp */
void uart_puts (char *s)
{
	while (*s)
	{   /* so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)" */
		uart_putc(*s);
		s++;
	}
}