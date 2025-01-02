/*
 * MT8808.c
 *
 * Created: 29/09/2024 8:20:26 PM
 *  Author: sphome

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


#include <inttypes.h>
#include <MT8808.h>
#include <util/delay.h>
#include <pins.h>

#define MT8808_DELAY 3

void MT8808_reset(void){
	//start strobe
	PORTB |= 1 << MT_STROBE;
	PORTD |=1 << MT_RESET;
	_delay_us(MT8808_DELAY);
	PORTD &=~(1 << MT_RESET);
	//end strobe
	PORTB &=~(1 << MT_STROBE);	
}


void MT8808_switch(uint8_t addr, uint8_t state){		
	//set address
	PORTB =(PORTB & (~ADDR_MASK)) | (addr & ADDR_MASK);
	_delay_us(MT8808_DELAY); //tAS
	//start strobe
	PORTB |= 1 << MT_STROBE;
	//set data	
	if (state) PORTB |=1 << MT_DATA;	//set data
	else PORTB &=~(1 << MT_DATA);		//reset data
	_delay_us(MT8808_DELAY);
	//end strobe	
	PORTB &=~(1 << MT_STROBE);
}
