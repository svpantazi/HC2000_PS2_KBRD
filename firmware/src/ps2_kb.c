/*
 * ps2_kb.c
 *
 * Created: 29/09/2024 5:19:53 PM
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

 REFERENCE:  
  AVR313: Interfacing the PC AT Keyboard Application Note; Rev. 1235B–AVR–05/02
 */ 

#include <inttypes.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <MT8808.h> //before delay so F_CPU is defined
#include <util/delay.h>
#include <ps2_kb.h>
#include <scan_code_lookup.h>
#include <pins.h>


#define E_MODE_DELAY 30
#define MACRO_TYPE_DELAY 50


volatile static uint8_t ps2_scan_code;                // Holds the received scan code
volatile static uint8_t edge,bitcount;

typedef enum PS2_KEY_DECODE_STATE{
	PS2_STATE_IDLE_WAIT_FOR_EVENT,
	PS2_STATE_KEY_PRESSED,
	PS2_STATE_KEY_RELEASED
} key_decode_state_t;

volatile key_decode_state_t state;
volatile bool ps2_ext_key_code;
volatile bool ps2_RIGHT_SHIFT_key_PRESSED;
volatile bool zx_digit_symbol_shift;
volatile bool zx_digit_symbol_shifted;
volatile bool ps2_RIGHT_SHIFTED_symbols;

void init_kb(void){
	MCUCR=ISC10;                              //INT0 on falling edge
	edge = 0;                                // 0 = falling edge  1 = rising edge
	bitcount = 11;
	ps2_scan_code=0;
	MT8808_reset();
//by default keyboard starts in code set 3
	PORTD |=(1 << LED);	
	_delay_us(512);
	PORTD&=~(1 << LED);	

	state=PS2_STATE_IDLE_WAIT_FOR_EVENT;
	ps2_ext_key_code=false;
	ps2_RIGHT_SHIFT_key_PRESSED=false;
	zx_digit_symbol_shift=false;
	zx_digit_symbol_shifted=false;	
	ps2_RIGHT_SHIFTED_symbols=false;
}


ISR (INT0_vect) {
    if (!edge) {
		// Routine entered at falling edge
	    if ((bitcount < 11) && (bitcount > 2)) {
			// Bit 3 to 10 is data. Parity bit, start and stop bits are ignored.
		    ps2_scan_code = (ps2_scan_code >> 1);//shift right and stores 0 in bit 7
			if (PIND & (1<<KBD_DATA)) ps2_scan_code = ps2_scan_code | 0x80;  // Store a '1'
		}		
		// Set interrupt on rising edge (MCUCR=3)
	    MCUCR = ISC11;                            
	    edge = 1;	    
	 } else {
		// Routine entered at rising edge
		// Set interrupt on falling edge (MCUCR=2)
	    MCUCR = ISC10;                            
	    edge = 0;
	    if ((--bitcount) == 0) {
			// All bits received
		    bitcount = 11;
			//last_scan_code=scan_code;
		    decode();
			ps2_scan_code=PS2_NO_KEY;
	    }
    }	
}

void ps2_scan_code_to_mt8808_switch(uint8_t scan_code){
	uint16_t zx_key_code;
	uint8_t mt_addr_switch[2];
	
	if (ps2_ext_key_code) zx_key_code=pgm_read_word(&(PS2_E0_EXT_CODE_TO_ZX[scan_code]));
	else zx_key_code=pgm_read_word(&(PS2_CODE_TO_ZX[scan_code]));	
	
	ps2_ext_key_code=false;
	
	last_scan_code=scan_code;//for blinking the LED
	
	//high byte goes first in processing
	mt_addr_switch[0]=(uint8_t) (zx_key_code >> 8);
	//low byte goes second in processing
	mt_addr_switch[1]=(uint8_t) zx_key_code;

	//handle 6,7,8,9,0 symbol exception
	//this creates a problem with the underscore and single quote which are rewritten symbols with dedicated keys
	//when dedicated keys are pressed the symbol shift zx_digit_symbol_shift is NOT true
	void shift_digit_symbols(const uint8_t j){
		if ((mt_addr_switch[j] & 56)==32) mt_addr_switch[j]|=2;//increase row by 2; 6-> H
		else if ((mt_addr_switch[j] & 56)==16) mt_addr_switch[j]+=19;//8->B, increase row by 3 and column by 2
		else mt_addr_switch[j]+=8; //0->9, 9->8, 7->6
	}
	

	if (state==PS2_STATE_KEY_RELEASED) {
						
		state=PS2_STATE_IDLE_WAIT_FOR_EVENT;		
	
		if (zx_digit_symbol_shifted && ((mt_addr_switch[1] & 7)==4)) {
			shift_digit_symbols(1);
			zx_digit_symbol_shifted=false;
		}
			
		for (int8_t i=1; i>=0; i--) {						
			//adding the conditions made the CS (Alt) work when pressed continuously 
			//as opposed to pressing it and only working for one symbol, not keeping it down continuously
			if ((mt_addr_switch[i] & ZX_CAP_BIT)>0)	MT8808_switch(ZX_KEY_CAPS,0);
			if ((mt_addr_switch[i] & ZX_SYM_BIT)>0){
				if (mt_addr_switch[i]==ZX_KEY_SYM) zx_digit_symbol_shift=false;
				MT8808_switch(ZX_KEY_SYM,0);
			}
			if (mt_addr_switch[i]>0) MT8808_switch(mt_addr_switch[i],0);
		}
	}
	else {		
		state=PS2_STATE_KEY_PRESSED;				
		
		if ((zx_digit_symbol_shift || zx_digit_symbol_shifted) && ((mt_addr_switch[1] & 7)==4)) {			
			shift_digit_symbols(1);
			zx_digit_symbol_shifted=true;
		}		
			
		//code of key that was pressed; activate switches
		for (int8_t i=0; i<=1; i++) {						
			//this condition is important to avoid processing a NO KEY value
			if (mt_addr_switch[i]>0) {
				if ((mt_addr_switch[i] & ZX_CAP_BIT)>0) MT8808_switch(ZX_KEY_CAPS,1);
				//else if (i==1) MT8808_switch(ZX_KEY_CAPS,0); //turning off the CAPS?
				if ((mt_addr_switch[i] & ZX_SYM_BIT)>0)
				{
					//this refers to separate symbol shift key pressed
					if (mt_addr_switch[i]==ZX_KEY_SYM) zx_digit_symbol_shift=true;
					MT8808_switch(ZX_KEY_SYM,1);
				}
				MT8808_switch(mt_addr_switch[i],1);			
				if (i==0){
					//this is the first key so after a short delay
					 _delay_ms(E_MODE_DELAY);
					 //turn off the CAPS and SYM only if the next key has them off
					if ((mt_addr_switch[1] & ZX_CAP_BIT)==0) MT8808_switch(ZX_KEY_CAPS,0);
					if ((mt_addr_switch[1] & ZX_SYM_BIT)==0) MT8808_switch(ZX_KEY_SYM,0);					 
				}
			}
		}
	}				
}

void run_macro(const uint8_t * macro){
	cli();//disable global interrupts
	uint8_t i=0;
	uint8_t scode;
	do {
#ifdef MACRO_MEM_EE
		scode=eeprom_read_byte(&macro[i]);
#else
		scode=pgm_read_byte((PGM_P) &macro[i]);
#endif
		if (scode==0) break;//otherwise there is a delay at the end of the macro
		ps2_scan_code=scode;
		decode();//press
		_delay_ms(MACRO_TYPE_DELAY);		
		ps2_scan_code=0xF0;
		decode();//release
		ps2_scan_code=scode;		
		decode();
		_delay_ms(2*MACRO_TYPE_DELAY);//twice the delay so switching to E and repetition have enough time
		i++;
	}
	while (scode>0);
	sei();//enable global interrupts	
}

void decode(void){
	if (ps2_scan_code==0xE0) {
		if (ps2_ext_key_code){ //taking care of E0 after E0
			ps2_ext_key_code=false;
			MT8808_reset();
		}
		else ps2_ext_key_code=true;
	}
	else if (ps2_scan_code==0xF0) {		
		state=PS2_STATE_KEY_RELEASED;
	}
	//these are macros
	else if (ps2_scan_code==PS2_KEY_CODE_F1){
		run_macro(&PS2_MACRO_CPM_RUN[0]);
	}
	else if (ps2_scan_code==PS2_KEY_CODE_F2){
		run_macro(&PS2_MACRO_LOAD_FROM_DISK[0]);
	}
	else if (ps2_scan_code==PS2_KEY_CODE_RIGHT_SHIFT){
		if (state==PS2_STATE_KEY_RELEASED){
			ps2_RIGHT_SHIFT_key_PRESSED=false;		
			state=PS2_STATE_IDLE_WAIT_FOR_EVENT;				
		}
		else {
			state=PS2_STATE_KEY_PRESSED;
			ps2_RIGHT_SHIFT_key_PRESSED=true;
		}
	}
	//regular codes
	else if ((ps2_scan_code>12) && (ps2_scan_code<128)) { //132 scan codes, and 125 extended scan codes				
		/*	dec 1
				,	65	<	64
				.	73	>	72								
				'	82	"	81
				[	84	{	83
			inc 1
				-	78	_	79										
				=	85	+	86
				]	91	}	92
				\	93	|	94

			dec 3
				/	74	?	71
			inc 4
				;	76	:	80				
		*/
		//rewriting shifted symbol codes
		if (ps2_RIGHT_SHIFT_key_PRESSED || ps2_RIGHT_SHIFTED_symbols) {
			if ((ps2_scan_code==65) || (ps2_scan_code==73) || (ps2_scan_code==82)|| (ps2_scan_code==84)) ps2_scan_code--;
			else if ((ps2_scan_code==78) || (ps2_scan_code==85) || (ps2_scan_code==91)|| (ps2_scan_code==93)) ps2_scan_code++;
			else if (ps2_scan_code==74) ps2_scan_code-=3;
			else if (ps2_scan_code==76) ps2_scan_code+=4;
			if (state==PS2_STATE_KEY_RELEASED) ps2_RIGHT_SHIFTED_symbols=false;
			else ps2_RIGHT_SHIFTED_symbols=true;
		}
		ps2_scan_code_to_mt8808_switch(ps2_scan_code);		
	}	
	else { 
		ps2_ext_key_code=false;
		//init_kb();
		MT8808_reset();
	}	
}
