/*
Copyright (C) 2018  Alexander Wilhelmi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hoport_e that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define FOSC 8000000 // Clock Sport_eed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#define PORT_E_OUTPUT_MOTOR_RIGHT_PIN 7
int port_e_out_motor_right_pin_bitmask;
#define PORT_E_OUTPUT_MOTOR_LEFT_PIN 6
int port_e_out_motor_left_pin_bitmask;

#define PORT_E_SENSOR_PIN  5
int port_e_sensor_pin_bitmask;
#define PORT_E_SENSOR2_PIN  3
int port_e_sensor2_pin_bitmask;

#define PORT_E_SENSOR_LED_POWER_PIN 2
int port_e_sensor_led_power_pin_bitmask;
//#define PORT_E_SENSOR_LED_POWER2_PIN 2
//int port_e_sensor_led_power2_pin_bitmask;

#define PORT_C_MOTOR_CMD_GREEN_CABLE_PIN 4  
int port_c_motor_cmd_green_cable_pin_bitmask;

#define PORT_C_GREY_CABLE_PIN 2
int port_c_grey_cable_bitmask;

#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)



int port_c_pin_config_input;
int port_c_pin_config_output;
int port_c_input_state;
int port_c_output_state;
int port_e_pin_config_input;
int port_e_pin_config_output;
int port_e_input_state;
int port_e_output_state;

int pwm_state=0;
int sensorval;


void init_io(void){
       // #########     setup  bitmasks    ####################
       port_e_sensor_led_power_pin_bitmask =      1 << PORT_E_SENSOR_LED_POWER_PIN ;
       port_e_sensor_pin_bitmask =                1 << PORT_E_SENSOR_PIN ;
       port_e_sensor2_pin_bitmask =               1 << PORT_E_SENSOR2_PIN ;
     //port_e_sensor_led_power2_pin_bitmask =     1 << PORT_E_SENSOR_LED_POWER2_PIN ;
       port_e_out_motor_left_pin_bitmask  =       1 << PORT_E_OUTPUT_MOTOR_LEFT_PIN ; 
       port_e_out_motor_right_pin_bitmask =       1 << PORT_E_OUTPUT_MOTOR_RIGHT_PIN ;
       port_c_grey_cable_bitmask =                1 << PORT_C_GREY_CABLE_PIN ;
       port_c_motor_cmd_green_cable_pin_bitmask = 1 << PORT_C_MOTOR_CMD_GREEN_CABLE_PIN ;
       
       
       
       // ########      pin config        ####################
       // PORT E
       DDRC = 0x00;                     // all-input
       PORTC = 0x00;                    // tristate-em
       port_e_pin_config_output = port_e_sensor_led_power_pin_bitmask | port_e_out_motor_left_pin_bitmask | port_e_out_motor_right_pin_bitmask ; 
       DDRE = 0x00 | port_e_pin_config_output;
       // PORT C
       DDRC = 0x00;                     // all-input
       PORTC = 0x00 | port_e_sensor_pin_bitmask; // tristate-em, pullup for sensor
     //PORTC = 0x00 ; // tristate-em
       port_c_pin_config_output = port_c_grey_cable_bitmask;
       DDRC = 0x00 | port_e_pin_config_output;
}



void grey_cable_high(){
	 port_c_output_state = port_c_output_state | port_c_grey_cable_bitmask; 
         PORTC = port_c_output_state;
}
void grey_cable_low(){
	 port_c_output_state = port_c_output_state & ~port_c_grey_cable_bitmask; 
         PORTC = port_c_output_state;
}
void green_cable_high(){
	 port_c_output_state = port_c_output_state | port_c_motor_cmd_green_cable_pin_bitmask; 
         PORTC = port_c_output_state;
}
void green_cable_low(){
	 port_c_output_state = port_c_output_state & ~port_c_motor_cmd_green_cable_pin_bitmask; 
         PORTC = port_c_output_state;
}


void motor_stop(){
	 port_e_output_state = port_e_output_state & ~port_e_out_motor_right_pin_bitmask;
	 port_e_output_state = port_e_output_state & ~port_e_out_motor_left_pin_bitmask;
         PORTE = port_e_output_state;

}
void motor_left(){
	 port_e_output_state = port_e_output_state | port_e_out_motor_left_pin_bitmask;
         PORTE = port_e_output_state;
}
void motor_right(){
	 port_e_output_state = port_e_output_state | port_e_out_motor_right_pin_bitmask;
         PORTE = port_e_output_state;
}
void sleep(short count){
// use cpu nop instruction to sleep count times with nop
        for ( short i = 0 ;  i < count ; i++){
                _NOP();
        }
}

void enable_interrupt(){
          cli();
          TIMSK0 = 0 | (1 << OCIE0A);    //enable interrupt comperator A
          TCCR0A = 0 | (1 << CS02) | (1 << CS00) | (1 << WGM01); // clocksource = io-clock/1024
          //TCCR0A = 0 | (1 << CS01) ; // clocksource = io-clock/1024
          TCNT0  = 0;   //timer counter 0
          OCR0A = 250;  // count until this number     -> 8 Mhz / 1024 / 255 + 1 or so
          sei();
        }

void disable_interrupt(){
          cli();
          TIMSK0 = 0;
          TCCR0A = 0;
          sei();
        }

/*void setup_ADC(){
        ADMUX  |=   ( 1 << REFS0   );     // select ref
        ADMUX  |=   ( 1 << ADLAR   );     // left adjust , good for 8 bit, read ADCH first
        ADMUX  |=   ( 0b00001 << 3 );     // select adc1
        ADMUX  |=   (   0b111 << 5 );     // prescaler 1/128
        DIDR1  |=   ( 1 << AIN1D   );     // no digital funktion -> saves power
        
        ADCSRA |=   ( 1 << ADEN    );     //enable adc
        ADCSRA |=   ( 1 << ADSC    );     //start conversion
}

int read_ADC(){
        if ( ( ADCSRA >> ADSC ) == 0 ) {
              return ADCH;
        }
        else{
                return 0;
        } 
}
*/

void setup_AC(){
      ADCSRB |=    ( 1 << ACME   ) ;//  multiplex on
      ADCSRA &=   ~( 1 << ADEN   ) ;//  adc off
       ADMUX |=    (  0b010      ) ;// select adc2
        ACSR &=   ~( 1 << ACD    ) ;// not disabled
        ACSR |=    ( 1 << ACIE   ) ;// enable interrupt
        ACSR &=   ~( 1 << ACIS0  ) ;// falling edge
        ACSR |=    ( 1 << ACIS1  ) ;// falling edge
}

ISR(ANALOG_COMP_vect){
                        grey_cable_high();
}

void sensor_power_on(){
	 port_e_output_state = port_e_output_state | port_e_sensor_led_power_pin_bitmask  | port_e_sensor_pin_bitmask;
	 //port_e_output_state = port_e_output_state & ~port_e_sensor_led_power2_pin_bitmask;
	// port_e_output_state = port_e_output_state | port_e_sensor_led_power2_pin_bitmask ;
         PORTE = port_e_output_state;
        //enable_interrupt();
}
void sensor_power_off(){
	 port_e_output_state = port_e_output_state & ~port_e_sensor_led_power_pin_bitmask;
         PORTE = port_e_output_state;
         disable_interrupt();
}
ISR(TIMER0_COMP_vect) {
         port_e_output_state = ( port_e_output_state & ( ~port_e_sensor_led_power_pin_bitmask ) ) | ( ~ ( port_e_sensor_led_power_pin_bitmask & port_e_output_state ) ) ; // port_with_pin_of | inverted pin -> pin toggle
         PORTE = port_e_output_state;
}

int main(void){
	init_io();
        sensor_power_on();
        setup_AC();
	while (1){
                if ( (port_c_motor_cmd_green_cable_pin_bitmask & PINC) == port_c_motor_cmd_green_cable_pin_bitmask ){
                        motor_left();
                }
                else{
                        motor_stop();
                }


                //port_c_output_state = (( (PINE & port_e_sensor_pin_bitmask ) >> PORT_E_SENSOR_PIN ) << PORT_C_GREY_CABLE_PIN) | ( port_c_output_state & ~port_c_grey_cable_bitmask);
                //PORTC = port_c_output_state;
        }
	return 0;
}
