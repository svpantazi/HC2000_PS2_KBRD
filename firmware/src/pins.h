/*
 * pins.h
 *
 * Created: 29/09/2024 8:21:06 PM
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


#ifndef PINS_H_
#define PINS_H_

#include <avr/io.h>

#define AX0			PB0
#define AX1			PB1
#define AX2			PB2
#define AY0			PB3
#define AY1			PB4
#define AY2			PB5
#define MT_STROBE	PB6
#define MT_DATA		PB7	//1 turns on selected switch, 0 turns off selected switch

#define MT_RESET	PD0 //output turns off all switches
#define LED			PD1 //output
#define KBD_CLK		PD2	//input
#define MOUSE_CLK	PD3	//input
#define KBD_DATA	PD4	//input
#define MOUSE_DATA	PD5	//input
#define UNUSED_IO	PD6	//input




#endif /* PINS_H_ */
