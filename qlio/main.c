/* make */
/* sudo avrdude -p t2313 -c avr910 -P /dev/ttyUSB0 -U flash:w:main.hex */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define BUFLEN  66

#define BAUD   191 /* baudrate prescaler, for baudrate 2400 */

#define OUTPUT   0
#define INPUT    1
#define TOGGLE   2
#define HIGH     3
#define LOW      4
#define READ     5

/* function definitions */
unsigned char rwpin(unsigned char, unsigned char);
unsigned char read_ana_comp(void);
unsigned char uart_receive(void);
void uart_transmit(unsigned char);
void setpwm(unsigned char, unsigned char);
void outstr(char *, void (*)(unsigned char));
char *instr(char *, unsigned char, char, unsigned char (*)(void));

int main(void)
{
	unsigned char cpc = 0;
	unsigned char iv = 0;
	char inbuf[BUFLEN];
	char *cv;

	/* uart: 8 data bits, no parity, 1 stop bit (8N1) */
	UBRRH = (unsigned char)(BAUD >> 8);
	UBRRL = (unsigned char)BAUD;

	UCSRB = (1 << RXEN) | (1 << TXEN);
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0);

	/* pwm */
	TCCR0A = (1 << WGM01) | (1 << WGM00); /* | (1 << COM0A1) | (1 << COM0B1); */
	TCCR0B = (1 << CS01);

	OCR0A = 0;
	OCR0B = 0;

	TCCR1A = (1 << WGM10); /* | (1 << COM1A1) | (1 << COM1B1); */
	TCCR1B = (1 << WGM12) | (1 << CS11);

	OCR1A = 0;
	OCR1B = 0;

	asm volatile("sei");

	while(1)
	{
		instr(inbuf, BUFLEN, '\n', uart_receive);
		cv = &(inbuf[1]);
		iv = atoi(cv); /* convert string to integer */;

		switch(inbuf[0])
		{
			case 'e':
				outstr(cv, uart_transmit);
				uart_transmit('\n');
				break;

			case 'i':
				rwpin(iv, INPUT);
				break;

			case 'o':
				rwpin(iv, OUTPUT);
				break;

			case 't':
				rwpin(iv, TOGGLE);
				break;

			case 'h':
				rwpin(iv, HIGH);
				break;

			case 'l':
				rwpin(iv, LOW);
				break;

			case 'r':
				iv = rwpin(iv, READ);
				outstr(itoa(iv, inbuf, 10), uart_transmit);
				uart_transmit('\n');
				break;

			case 'a':
				iv = read_ana_comp();
				outstr(itoa(iv, inbuf, 10), uart_transmit);
				uart_transmit('\n');
				break;

			case 'c':
				cpc = iv;
				break;

			case 'p':
				setpwm(cpc, iv);
				break;
		}
	}

	return 0;
}

unsigned char read_ana_comp()
{
	return ((ACSR >> ACO) & 1);
}

void setpwm(unsigned char ch, unsigned char val)
{
	if(ch >= 4) return;

	if(val)
	{
		/* enable pwma and write value to register */
		switch(ch)
		{
			case 0:
				TCCR0A |= (1 << COM0A1);
				OCR0A = val;
				break;

			case 1:
				TCCR0A |= (1 << COM0B1);
				OCR0B = val;
				break;

			case 2:
				TCCR1A |= (1 << COM1A1);
				OCR1A = val;
				break;

			case 3:
				TCCR1A |= (1 << COM1B1);
				OCR1B = val;
				break;
		}
	}
	else
	{
		/* disable pwm */
		switch(ch)
		{
			case 0:
				TCCR0A &= ~(1 << COM0A1);
				break;

			case 1:
				TCCR0A &= ~(1 << COM0B1);
				break;

			case 2:
				TCCR1A &= ~(1 << COM1A1);
				break;

			case 3:
				TCCR1A &= ~(1 << COM1B1);
				break;
		}
	}
}

/* digital io */
unsigned char rwpin(unsigned char p, unsigned char a)
{
	unsigned char map_pin;
	volatile unsigned char *reg_ddr;
	volatile unsigned char *reg_pin;
	volatile unsigned char *reg_port;

	if(p >= 13) return 0;

	if(p <= 7)
	{
		reg_ddr = &DDRB;
		reg_pin = &PINB;
		reg_port = &PORTB;
		map_pin = p;
	}
	else if(p >= 8 && p <= 12)
	{
		reg_ddr = &DDRD;
		reg_pin = &PIND;
		reg_port = &PORTD;
		map_pin = p-6;
	}

	switch(a)
	{
		case OUTPUT:
			*reg_ddr |= (1 << map_pin);
			break;

		case INPUT:
			*reg_ddr &= ~(1 << map_pin);
			break;

		case TOGGLE:
			*reg_port ^= (1 << map_pin);
			break;

		case HIGH:
			*reg_port |= (1 << map_pin);
			break;

		case LOW:
			*reg_port &= ~(1 << map_pin);
			break;

		case READ:
			return ((*reg_pin >> map_pin) & 1);
			break;
	}

	return 0;
}

/* uart */
unsigned char uart_receive(void)
{
	while (!(UCSRA & (1 << RXC))) { }
	return UDR;
}

void uart_transmit(unsigned char c)
{
	while (!(UCSRA & (1 << UDRE))) { }
	UDR = c;
}

void outstr(char *str, void (*fn_out)(unsigned char))
{
	while(*str)
	{
		(*fn_out)(*str++);
	}
}

char *instr(char *buf, unsigned char buflen, char term, unsigned char (*fn_in)(void))
{
	unsigned char i = 0;
	unsigned char c;

	while((c = (*fn_in)()) != term && i < buflen-1)
	{
		if(c >= 32)
		{
			buf[i++] = (char)c;
		}
	}

	buf[i] = '\0';
	return buf;
}
