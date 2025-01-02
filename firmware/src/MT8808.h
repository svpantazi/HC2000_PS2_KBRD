/*
 * MT8808.h
 *
 * Created: 29/09/2024 8:23:07 PM
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


#ifndef MT8808_H_
#define MT8808_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#include <inttypes.h>

#define ADDR_MASK 0x3f //data,strobe,AY2,AY1,AY0,AX2,AX1,AX0

void MT8808_reset(void);
void MT8808_switch(uint8_t addr, uint8_t state);

#endif /* MT8808_H_ */
