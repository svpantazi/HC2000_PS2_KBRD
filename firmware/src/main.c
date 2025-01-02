/*
 * HC2k_PS2_kbrd_ATTiny2313.c
 *
 * Created: 12/07/2024 9:19:39 PM
 * Author : sphome

    Copyright (C) 2024 Stefan V. Pantazi (svpantazi@gmail.com)    
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/.

 */ 

//see avrlib and examples

#include <avr/io.h>
#include <MT8808.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <pins.h>
#include <ps2_kb.h>



void init_ports(void){
	//set port B to output
	DDRB=0xff;
	//set all port D bits to 0
	PORTB=0x00;
	
	//set port D IO directions; 1 is output, 0 is input
	DDRD=0 | (1<<MT_RESET) | (1<<LED);
	//set output port D bits to 0
	PORTD=0xff & ~(1<<MT_RESET);
}


void blink_led(void){	
	for (int8_t i=7; i>=0; i--) {
		PORTD |=(1 << LED);	
		if ((last_scan_code & (1 << i))>0) _delay_ms(32);
		else _delay_ms(128);
		PORTD&=~(1 << LED);		
		_delay_ms(128);
	}	
	PORTD &=~(1 << LED);
	_delay_ms(512);
}


int main(void)
{
	init_ports();
	MT8808_reset();
	
	init_kb();		
	
	GIMSK|=1<<INT0; //GIMSK=0x40; enable int0
	
	sei();//enable global interrupts
	
    while (1) 
    {		
		blink_led();
    }
}

