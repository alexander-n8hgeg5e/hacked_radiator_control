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
int pe_o_sensor_power_port_bitmask = 1 << 2 ;

// port e 6,7 : "1" motor control output
int pe_o_motor_left_port_bitmask  = 1 << 6 ; 
int pe_o_motor_right_port_bitmask = 1 << 7 ;

// port e 5 : "0" setup increment photo transistor as input
int pe_i_sensor_port_bitmask = 1 << 5 ;

//grey cable is sensor forwarding output
//is port c2
int pc_o_sensor_out_bitmask = 0b00000100;


//port init
  //to write whole port at once, need the vars for this
  int write;
  int output;


  // port e
  output = pe_o_sensor_power_port_bitmask || pe_o_motor_left_port_bitmask || pe_o_motor_right_port_bitmask ; 
  DDRE = 0b00000000 || output; // if not output -> input
  porte = output; //set outputs high, no more tristate
  // port c
  output = pc_o_sensor_out_bitmask ;
  DDRC = 0b00000000 || output; //if not output -> input 
  portc = output; //set outputs high, no more tristate
 

}

int main(void)
{
	init_io();
	while (1)
	{
      // port e
      // sensor power high
	  write = pe_o_sensor_power_port_bitmask ;
	  // motor left signal low
	  write =   !write   || pe_o_motor_left_port_bitmask ;
      //write port
	  porte = write;
      
	  //port c
      write =  pc_o_sensor_out_bitmask && ( pe_i_sensor_port_bitmask && porte ) ;
	}

	return 0;
}
