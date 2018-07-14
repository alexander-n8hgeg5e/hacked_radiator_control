/*
Copyright (C) 2018  Alexander Wilhelmi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <util/delay.h>
#define FOSC 8000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
void init_io(void)
{
  //DDRD = 0b11111111;
  DDRE = 0b11000000; // motor 

}

void USART_Init( unsigned int ubrr)
{
/* Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
/* Set frame format: 8data, 2stop bit */
UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

int main(void)
{
	init_io();
        USART_Init(MYUBRR)
	while (1)
	{
	        int d = 10;	
		_delay_ms(d);
		PORTE = 0b01111111;
		_delay_ms(d);
		PORTE = 0b00111111;
		_delay_ms(d);
		PORTE = 0b10111111;
	        d = 20;	
		_delay_ms(d);
		PORTE = 0b01111111;
		_delay_ms(d);
		PORTE = 0b00111111;
		_delay_ms(d);
		PORTE = 0b10111111;
	        d = 50;	
		_delay_ms(d);
		PORTE = 0b01111111;
		_delay_ms(d);
		PORTE = 0b00111111;
		_delay_ms(d);
		PORTE = 0b10111111;
	        d = 80;	
		_delay_ms(d);
		PORTE = 0b01111111;
		_delay_ms(d);
		PORTE = 0b00111111;
		_delay_ms(d);
		PORTE = 0b10111111;
	        d = 90;	
		_delay_ms(d);
		PORTE = 0b01111111;
		_delay_ms(d);
		PORTE = 0b00111111;
		_delay_ms(d);
		PORTE = 0b10111111;
	}

	return 0;
}
