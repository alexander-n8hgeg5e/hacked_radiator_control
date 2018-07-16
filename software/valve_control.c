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
// port e 2: "1" increment sensor led output
// port e 6,7 : "1" motor control output
// port e 5 : "0" setup increment photo transistor as input
  DDRE = 0b11000010;
  DDRD = 0b00100000;
}

void USART_Init( unsigned int ubrr)
{
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

int main(void)
{
	init_io();
        USART_Init(MYUBRR)
	while (1)
	{
		PORTE = 0b10111111;
		PORTD = 
	}

	return 0;
}
