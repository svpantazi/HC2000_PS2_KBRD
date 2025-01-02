/*
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
#include <avr/pgmspace.h>

#define PS2_NO_KEY 0x00

#define ZX_SYM_BIT 0x80
#define ZX_CAP_BIT 0x40
//,e_mode,g_mode

#define ZX_KEY(row,col) (0x00 | (col << 3) | row)

#define ZX_ONE_KEY(k)		(k	| 0x0000	)
#define ZX_TWO_KEY(k1,k2)	(k2	| (k1 << 8)	)
#define ZX_CAP(k)			(k	| ZX_CAP_BIT)
#define ZX_SYM(k)			(k	| ZX_SYM_BIT)


/* ZX spectrum key matrix
				
		TD0		TD1		TD2		TD3		TD4		AX[0..2]			EX5		EX6		EX7
 A8		CAPS	Z		X		C		V		0, 0, 0		+0
 A9		A		S		D		F		G		1, 0, 0		+1
A10		Q		W		E		R		T		0, 1, 0		+2
A11		1		2		3		4		5		1, 1, 0		+3
A12		0		9		8		7		6		0, 0, 1		+4
A13		P		O		I		U		Y		1, 0, 1		+5		
A14		CR		L		K		J		H		0, 1, 1		+6
A15		SP		SYM		M		N		B		1, 1, 1		+7

		+0		+1		+2		+3		+4							+5		+6		+7
AY0		0		1		0		1		0							1		0		1
AY1		0		0		1		1		0							0		1		1
AY2		0		0		0		0		1							1		1		1
	
if AY[0..2] is 5, 6 or 7, then special handling is signaled				
*/

#define ZX_KEY_CAPS		ZX_ONE_KEY(ZX_CAP(ZX_KEY(0,0))) //must OR with cap bit
#define ZX_KEY_Z		ZX_ONE_KEY(ZX_KEY(0,1))
#define ZX_KEY_X		ZX_ONE_KEY(ZX_KEY(0,2))
#define ZX_KEY_C		ZX_ONE_KEY(ZX_KEY(0,3))
#define ZX_KEY_V		ZX_ONE_KEY(ZX_KEY(0,4))
											 
#define ZX_KEY_A		ZX_ONE_KEY(ZX_KEY(1,0))
#define ZX_KEY_S		ZX_ONE_KEY(ZX_KEY(1,1))
#define ZX_KEY_D		ZX_ONE_KEY(ZX_KEY(1,2))
#define ZX_KEY_F		ZX_ONE_KEY(ZX_KEY(1,3))
#define ZX_KEY_G		ZX_ONE_KEY(ZX_KEY(1,4))
											 
#define ZX_KEY_Q		ZX_ONE_KEY(ZX_KEY(2,0))
#define ZX_KEY_W		ZX_ONE_KEY(ZX_KEY(2,1))
#define ZX_KEY_E		ZX_ONE_KEY(ZX_KEY(2,2))
#define ZX_KEY_R		ZX_ONE_KEY(ZX_KEY(2,3))
#define ZX_KEY_T		ZX_ONE_KEY(ZX_KEY(2,4))
											 
#define ZX_KEY_1		ZX_ONE_KEY(ZX_KEY(3,0))
#define ZX_KEY_2		ZX_ONE_KEY(ZX_KEY(3,1))
#define ZX_KEY_3		ZX_ONE_KEY(ZX_KEY(3,2))
#define ZX_KEY_4		ZX_ONE_KEY(ZX_KEY(3,3))
#define ZX_KEY_5		ZX_ONE_KEY(ZX_KEY(3,4))
						
#define ZX_KEY_0		ZX_ONE_KEY(ZX_KEY(4,0))
#define ZX_KEY_9		ZX_ONE_KEY(ZX_KEY(4,1))
#define ZX_KEY_8		ZX_ONE_KEY(ZX_KEY(4,2))
#define ZX_KEY_7		ZX_ONE_KEY(ZX_KEY(4,3))
#define ZX_KEY_6		ZX_ONE_KEY(ZX_KEY(4,4))
						
#define ZX_KEY_P		ZX_ONE_KEY(ZX_KEY(5,0))
#define ZX_KEY_O		ZX_ONE_KEY(ZX_KEY(5,1))
#define ZX_KEY_I		ZX_ONE_KEY(ZX_KEY(5,2))
#define ZX_KEY_U		ZX_ONE_KEY(ZX_KEY(5,3))
#define ZX_KEY_Y		ZX_ONE_KEY(ZX_KEY(5,4))
						
#define ZX_KEY_CR		ZX_ONE_KEY(ZX_KEY(6,0))
#define ZX_KEY_L		ZX_ONE_KEY(ZX_KEY(6,1))
#define ZX_KEY_K		ZX_ONE_KEY(ZX_KEY(6,2))
#define ZX_KEY_J		ZX_ONE_KEY(ZX_KEY(6,3))
#define ZX_KEY_H		ZX_ONE_KEY(ZX_KEY(6,4))
						
#define ZX_KEY_SP		ZX_ONE_KEY(ZX_KEY(7,0))
#define ZX_KEY_SYM		ZX_ONE_KEY(ZX_SYM(ZX_KEY(7,1))) //I OR the SYM bit
#define ZX_KEY_M		ZX_ONE_KEY(ZX_KEY(7,2))
#define ZX_KEY_N		ZX_ONE_KEY(ZX_KEY(7,3))
#define ZX_KEY_B		ZX_ONE_KEY(ZX_KEY(7,4))
						
//CAPS+key				
#define ZX_KEY_EDIT		ZX_ONE_KEY(ZX_CAP(ZX_KEY_1)) //same as escape 
#define ZX_KEY_ESCAPE	ZX_ONE_KEY(ZX_CAP(ZX_KEY_1)) //same as edit

#define ZX_KEY_CAPS_LCK	ZX_ONE_KEY(ZX_CAP(ZX_KEY_2)) //
#define ZX_KEY_NORM_VID	ZX_ONE_KEY(ZX_CAP(ZX_KEY_3))
#define ZX_KEY_INV_VID	ZX_ONE_KEY(ZX_CAP(ZX_KEY_4))
#define ZX_KEY_LEFT		ZX_ONE_KEY(ZX_CAP(ZX_KEY_5)) //
						
#define ZX_KEY_DEL		ZX_ONE_KEY(ZX_CAP(ZX_KEY_0)) //
#define ZX_KEY_GR_MODE	ZX_ONE_KEY(ZX_CAP(ZX_KEY_9)) //
#define ZX_KEY_RIGHT	ZX_ONE_KEY(ZX_CAP(ZX_KEY_8)) //
#define ZX_KEY_UP		ZX_ONE_KEY(ZX_CAP(ZX_KEY_7)) //
#define ZX_KEY_DOWN		ZX_ONE_KEY(ZX_CAP(ZX_KEY_6)) //

//SYM + key
#define ZX_KEY_EXT_MODE ZX_ONE_KEY(ZX_SYM(ZX_KEY_CAPS)) //CAP+SYM
#define ZX_KEY_COLON	ZX_ONE_KEY(ZX_SYM(ZX_KEY_Z))
#define ZX_KEY_POUND	ZX_ONE_KEY(ZX_SYM(ZX_KEY_X))
#define ZX_KEY_QMARK	ZX_ONE_KEY(ZX_SYM(ZX_KEY_C))
#define ZX_KEY_SLASH	ZX_ONE_KEY(ZX_SYM(ZX_KEY_V))
#define ZX_KEY_STAR		ZX_ONE_KEY(ZX_SYM(ZX_KEY_B))
#define ZX_KEY_COMMA	ZX_ONE_KEY(ZX_SYM(ZX_KEY_N))
#define ZX_KEY_PERIOD	ZX_ONE_KEY(ZX_SYM(ZX_KEY_M))

#define ZX_KEY_HAT		ZX_ONE_KEY(ZX_SYM(ZX_KEY_H))
#define ZX_KEY_MINUS	ZX_ONE_KEY(ZX_SYM(ZX_KEY_J))
#define ZX_KEY_PLUS		ZX_ONE_KEY(ZX_SYM(ZX_KEY_K))
#define ZX_KEY_EQUAL	ZX_ONE_KEY(ZX_SYM(ZX_KEY_L))

#define ZX_KEY_LTEQ		ZX_ONE_KEY(ZX_SYM(ZX_KEY_Q))
#define ZX_KEY_DIFF		ZX_ONE_KEY(ZX_SYM(ZX_KEY_W))
#define ZX_KEY_GTEQ		ZX_ONE_KEY(ZX_SYM(ZX_KEY_E))
#define ZX_KEY_ANG_BRACKET_OPEN		ZX_ONE_KEY(ZX_SYM(ZX_KEY_R))
#define ZX_KEY_ANG_BRACKET_CLOSE	ZX_ONE_KEY(ZX_SYM(ZX_KEY_T))

#define ZX_KEY_SEMICOLON	ZX_ONE_KEY(ZX_SYM(ZX_KEY_O))
#define ZX_KEY_DOUBLE_QUOTE	ZX_ONE_KEY(ZX_SYM(ZX_KEY_P))

#define ZX_KEY_EXCL					ZX_ONE_KEY(ZX_SYM(ZX_KEY_1))
#define ZX_KEY_AT					ZX_ONE_KEY(ZX_SYM(ZX_KEY_2))
#define ZX_KEY_HASH					ZX_ONE_KEY(ZX_SYM(ZX_KEY_3))
#define ZX_KEY_DOLLAR				ZX_ONE_KEY(ZX_SYM(ZX_KEY_4))
#define ZX_KEY_PERCENT				ZX_ONE_KEY(ZX_SYM(ZX_KEY_5))
#define ZX_KEY_AMPER				ZX_ONE_KEY(ZX_SYM(ZX_KEY_6))
#define ZX_KEY_SINGLE_QUOTE			ZX_ONE_KEY(ZX_SYM(ZX_KEY_7))
#define ZX_KEY_ROUND_BRACKET_OPEN	ZX_ONE_KEY(ZX_SYM(ZX_KEY_8))
#define ZX_KEY_ROUND_BRACKET_CLOSE	ZX_ONE_KEY(ZX_SYM(ZX_KEY_9))
#define ZX_KEY_UNDERSCORE			ZX_ONE_KEY(ZX_SYM(ZX_KEY_0))

//#define ZX_KEY_CAP_BRK	ZX_ONE_KEY(ZX_KEY())

//E mode + key
#define ZX_KEY_TAB					ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_KEY_P)
#define ZX_KEY_USR					ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_KEY_L)

//E mode + SYM + key
#define ZX_KEY_SQ_BRACKET_OPEN		ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_SYM(ZX_KEY_Y))
#define ZX_KEY_SQ_BRACKET_CLOSE		ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_SYM(ZX_KEY_U))
#define ZX_KEY_COPYRIGHT			ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_SYM(ZX_KEY_P))

#define ZX_KEY_TILDE				ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_SYM(ZX_KEY_A))
#define ZX_KEY_PIPE					ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_SYM(ZX_KEY_S))
#define ZX_KEY_BACKSLASH			ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_SYM(ZX_KEY_D))
#define ZX_KEY_CURL_BRACKET_OPEN	ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_SYM(ZX_KEY_F))
#define ZX_KEY_CURL_BRACKET_CLOSE	ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_SYM(ZX_KEY_G))

#define ZX_KEY_CAT					ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_SYM(ZX_KEY_9))

#define ZX_KEY_CTRL					ZX_TWO_KEY(ZX_KEY_EXT_MODE,	ZX_CAP(ZX_SYM(0))) //????


/*
https://wiki.osdev.org/PS/2_Keyboard
Note:	There	is	no	scan	code	for	pause key released	(it	behaves	as	if	it	is	released	as	soon	as	it's	pressed)
0xE1	0x14	0x77	0xE1	0xF0	0x14	0xF0	0x77	pause
0xE0	0xF0	0x7C	0xE0	0xF0	0x12	print	screen	released
*/

//			Scan	Code	Set	2	make codes;	//break codes	prefixed by 0xF0
const PROGMEM uint16_t PS2_CODE_TO_ZX[]={
//col, row	
		0x00,  //	0	0x0	0x00	no key
		0x00,  //	1	0x1	0x01	F9
		0x00,  //	2	0x2
		0x00,  //	3	0x3	0x03	F5
		0x00,  //	4	0x4	0x04	F3
		0x00,  //	5	0x5	0x05	F1
		0x00,  //	6	0x6	0x06	F2
		0x00,  //	7	0x7	0x07	F12 
		0x00,  //	8	0x8
		0x00,  //	9	0x9	0x09	F10
		0x00,  //	10	0xA	0x0A	F8
		0x00,  //	11	0xB	0x0B	F6
		0x00,  //	12	0xC	0x0C	F4
	ZX_KEY_TAB,  //	13	0xD	0x0D	tab
	ZX_KEY_TILDE,  //	14	0xE	0x0E	`	(back	tick); no back tick in Spectrum; tilda with no shift
	ZX_KEY_USR,  //	15	0xF						#### PS2 unused code, assign to USR macro; WORKS
		0x00,  //	16	0x10
	ZX_KEY_SYM,  //	17	0x11	0x11	left	alt
	ZX_KEY_CAPS,  //	18	0x12	0x12	left	shift
		0x00,  //	19	0x13
	ZX_KEY_CTRL,  //	20	0x14	0x14	left	control #### EXT MODE
	ZX_KEY_Q,  //	21	0x15	0x15	Q
	ZX_KEY_1,  //	22	0x16	0x16	1
		0x00,  //	23	0x17
		0x00,  //	24	0x18
		0x00,  //	25	0x19
	ZX_KEY_Z,  //	26	0x1A	0x1A	Z
	ZX_KEY_S,  //	27	0x1B	0x1B	S
	ZX_KEY_A,  //	28	0x1C	0x1C	A
	ZX_KEY_W,  //	29	0x1D	0x1D	W
	ZX_KEY_2,  //	30	0x1E	0x1E	2
		0x00,  //	31	0x1F
		0x00,  //	32	0x20
	ZX_KEY_C,  //	33	0x21	0x21	C
	ZX_KEY_X,  //	34	0x22	0x22	X
	ZX_KEY_D,  //	35	0x23	0x23	D
	ZX_KEY_E,  //	36	0x24	0x24	E
	ZX_KEY_4,  //	37	0x25	0x25	4
	ZX_KEY_3,  //	38	0x26	0x26	3
		0x00,  //	39	0x27
		0x00,  //	40	0x28
	ZX_KEY_SP,  //	41	0x29	0x29	space
	ZX_KEY_V,  //	42	0x2A	0x2A	V
	ZX_KEY_F,  //	43	0x2B	0x2B	F
	ZX_KEY_T,  //	44	0x2C	0x2C	T
	ZX_KEY_R,  //	45	0x2D	0x2D	R
	ZX_KEY_5,  //	46	0x2E	0x2E	5
		0x00,  //	47	0x2F	
		0x00,  //	48	0x30
	ZX_KEY_N,  //	49	0x31	0x31	N
	ZX_KEY_B,  //	50	0x32	0x32	B
	ZX_KEY_H,  //	51	0x33	0x33	H
	ZX_KEY_G,  //	52	0x34	0x34	G
	ZX_KEY_Y,  //	53	0x35	0x35	Y
	ZX_KEY_6,  //	54	0x36	0x36	6
		0x00,  //	55	0x37
		0x00,  //	56	0x38
		0x00,  //	57	0x39
	ZX_KEY_M,  //	58	0x3A	0x3A	M
	ZX_KEY_J,  //	59	0x3B	0x3B	J
	ZX_KEY_U,  //	60	0x3C	0x3C	U
	ZX_KEY_7,  //	61	0x3D	0x3D	7
	ZX_KEY_8,  //	62	0x3E	0x3E	8
		0x00,  //	63	0x3F
	ZX_KEY_ANG_BRACKET_OPEN,  //	64	0x40											### PS2 unused code assigned to < macro
	ZX_KEY_COMMA,  //	65	0x41	0x41  comma ,
	ZX_KEY_K,  //	66	0x42	0x42	K
	ZX_KEY_I,  //	67	0x43	0x43	I
	ZX_KEY_O,  //	68	0x44	0x44	O
	ZX_KEY_0,  //	69	0x45	0x45	0	(zero)
	ZX_KEY_9,  //	70	0x46	0x46	9
	ZX_KEY_QMARK,  //	71	0x47											### PS2 unused code assigned to ? macro
	ZX_KEY_ANG_BRACKET_CLOSE,  //	72	0x48											### PS2 unused code assigned to > macro
	ZX_KEY_PERIOD,  //	73	0x49	0x49	.
	ZX_KEY_SLASH,  //	74	0x4A	0x4A	/
	ZX_KEY_L,  //	75	0x4B	0x4B	L
	ZX_KEY_SEMICOLON,  //	76	0x4C	0x4C	;
	ZX_KEY_P,  //	77	0x4D	0x4D	P
	ZX_KEY_MINUS,  //	78	0x4E	0x4E	-
	ZX_KEY_UNDERSCORE,  //	79	0x4F											### PS2 unused code assigned to _ macro
	ZX_KEY_COLON,  //	80	0x50											### PS2 unused code assigned to : macro
	ZX_KEY_DOUBLE_QUOTE,  //	81	0x51								### PS2 unused code assigned to " macro
	ZX_KEY_SINGLE_QUOTE,  //	82	0x52	0x52	'
	ZX_KEY_CURL_BRACKET_OPEN,  //	83	0x53											### PS2 unused code assigned to { macro
	ZX_KEY_SQ_BRACKET_OPEN,  //	84	0x54	0x54	[
	ZX_KEY_EQUAL,  //	85	0x55	0x55	=
	ZX_KEY_PLUS,  //	86	0x56											### PS2 unused code assigned to + macro
		0x00,  //	87	0x57
	ZX_KEY_CAPS_LCK,  //	88	0x58	0x58	CapsLock
		0x00,  //	89	0x59	0x59	right	shift						- not CAPS but shift for Symbols on PS2 keyboard
	ZX_KEY_CR,  //	90	0x5A	0x5A	enter
	ZX_KEY_SQ_BRACKET_CLOSE,  //	91	0x5B	0x5B	]
	ZX_KEY_CURL_BRACKET_CLOSE,  //	92	0x5C											### PS2 unused code assigned to } macro
	ZX_KEY_BACKSLASH,  //	93	0x5D	0x5D	backslash, 
	ZX_KEY_PIPE,  //	94	0x5E											### PS2 unused code assigned to | macro
		0x00,  //	95	0x5F											
		0x00,  //	96	0x60											
		0x00,  //	97	0x61											
		0x00,  //	98	0x62
		0x00,  //	99	0x63
		0x00,  //	100	0x64
		0x00,  //	101	0x65
	ZX_KEY_DEL,  //	102	0x66	0x66	backspace
		0x00,  //	103	0x67
		0x00,  //	104	0x68
	ZX_KEY_1,  //	105	0x69	0x69	(keypad)	1
		0x00,  //	106	0x6A
	ZX_KEY_4,  //	107	0x6B	0x6B	(keypad)	4
	ZX_KEY_7,  //	108	0x6C	0x6C	(keypad)	7
		0x00,  //	109	0x6D
		0x00,  //	110	0x6E
		0x00,  //	111	0x6F
	ZX_KEY_0,  //	112	0x70	0x70	(keypad)	0
	ZX_KEY_PERIOD,  //	113	0x71	0x71	(keypad)	.
	ZX_KEY_2,  //	114	0x72	0x72	(keypad)	2
	ZX_KEY_5,  //	115	0x73	0x73	(keypad)	5
	ZX_KEY_6,  //	116	0x74	0x74	(keypad)	6
	ZX_KEY_8,  //	117	0x75	0x75	(keypad)	8
	ZX_KEY_ESCAPE,  //	118	0x76	0x76	escape				#### CP/M???
	ZX_KEY_CAT,  //	119	0x77	0x77	NumberLock				# Basic CAT command
		0x00,  //	120	0x78	0x78	F11					
	ZX_KEY_PLUS,  //	121	0x79	0x79	(keypad)	+
	ZX_KEY_3,  //	122	0x7A	0x7A	(keypad)	3
	ZX_KEY_MINUS,  //	123	0x7B	0x7B	(keypad)	-
	ZX_KEY_STAR,  //	124	0x7C	0x7C	(keypad)	*
	ZX_KEY_9,  //	125	0x7D	0x7D	(keypad)	9
		0x00,  //	126	0x7E	0x7E	ScrollLock			#CP/M
		0x00,  //	127	0x7F
		0x00,  //	128	0x80
		0x00,  //	129	0x81
		0x00,  //	130	0x82
		0x00  //	131	0x83	0x83	F7
};

//Scan	Code	Set	2	extended 2	byte	scan	codes;	prefixed	by	0xE0; extended break codes are prefixed by	0xE0	0xF0
const PROGMEM uint16_t PS2_E0_EXT_CODE_TO_ZX[]={
	0x00,  //	0	0
	0x00,  //	1	1
	0x00,  //	2	2
	0x00,  //	3	3
	0x00,  //	4	4
	0x00,  //	5	5
	0x00,  //	6	6
	0x00,  //	7	7
	0x00,  //	8	8
	0x00,  //	9	9
	0x00,  //	10	A
	0x00,  //	11	B
	0x00,  //	12	C
	0x00,  //	13	D
	0x00,  //	14	E
	0x00,  //	15	F
	0x00,  //	16	10	0xE0	0x10	(multimedia)	WWW	search
	ZX_KEY_SYM,  //	17	11	0xE0	0x11	right	alt
	0x00,  //	18	12	0xE0	0x12	0xE0	0x7C	print	screen		SPECIAL
	0x00,  //	19	13
	ZX_KEY_CTRL,  //	20	14	0xE0	0x14	right	control									E mode
	0x00,  //	21	15	0xE0	0x15	(multimedia)	previous	track
	0x00,  //	22	16
	0x00,  //	23	17
	0x00,  //	24	18	0xE0	0x18	(multimedia)	WWW	favourites
	0x00,  //	25	19
	0x00,  //	26	1A
	0x00,  //	27	1B
	0x00,  //	28	1C
	0x00,  //	29	1D
	0x00,  //	30	1E
	0x00,  //	31	1F	0xE0	0x1F	left	GUI
	0x00,  //	32	20	0xE0	0x20	(multimedia)	WWW	refresh
	0x00,  //	33	21	0xE0	0x21	(multimedia)	volume	down
	0x00,  //	34	22
	0x00,  //	35	23	0xE0	0x23	(multimedia)	mute
	0x00,  //	36	24
	0x00,  //	37	25
	0x00,  //	38	26
	0x00,  //	39	27	0xE0	0x27	right	GUI
	0x00,  //	40	28	0xE0	0x28	(multimedia)	WWW	stop
	0x00,  //	41	29
	0x00,  //	42	2A
	0x00,  //	43	2B	0xE0	0x2B	(multimedia)	calculator
	0x00,  //	44	2C
	0x00,  //	45	2D
	0x00,  //	46	2E
	0x00,  //	47	2F	0xE0	0x2F	apps
	0x00,  //	48	30	0xE0	0x30	(multimedia)	WWW	forward
	0x00,  //	49	31
	0x00,  //	50	32	0xE0	0x32	(multimedia)	volume	up
	0x00,  //	51	33
	0x00,  //	52	34	0xE0	0x34	(multimedia)	play/pause
	0x00,  //	53	35
	0x00,  //	54	36
	0x00,  //	55	37	0xE0	0x37	(ACPI)	power
	0x00,  //	56	38	0xE0	0x38	(multimedia)	WWW	back
	0x00,  //	57	39
	0x00,  //	58	3A	0xE0	0x3A	(multimedia)	WWW	home
	0x00,  //	59	3B	0xE0	0x3B	(multimedia)	stop
	0x00,  //	60	3C
	0x00,  //	61	3D
	0x00,  //	62	3E
	0x00,  //	63	3F	0xE0	0x3F	(ACPI)	sleep
	0x00,  //	64	40	0xE0	0x40	(multimedia)	my	computer
	0x00,  //	65	41
	0x00,  //	66	42
	0x00,  //	67	43
	0x00,  //	68	44
	0x00,  //	69	45
	0x00,  //	70	46
	0x00,  //	71	47
	0x00,  //	72	48	0xE0	0x48	(multimedia)	email
	0x00,  //	73	49
	ZX_KEY_SLASH,  //	74	4A	0xE0	0x4A	(keypad)	/
	0x00,  //	75	4B
	0x00,  //	76	4C
	0x00,  //	77	4D	0xE0	0x4D	(multimedia)	next	track
	0x00,  //	78	4E
	0x00,  //	79	4F
	0x00,  //	80	50	0xE0	0x50	(multimedia)	media	select
	0x00,  //	81	51	
	0x00,  //	82	52
	0x00,  //	83	53
	0x00,  //	84	54
	0x00,  //	85	55	
	0x00,  //	86	56
	0x00,  //	87	57
	0x00,  //	88	58
	0x00,  //	89	59
	ZX_KEY_CR,  //	90	5A 0xE0	0x5A	(keypad)	enter
	0x00,  //	91	5B
	0x00,  //	92	5C	cannot have 0xE0	after E0 extended
	0x00,  //	93	5D	cannot have E1 after E0
	0x00,  //	94	5E  0xE0	0x5E	(ACPI)	wake
	0x00,  //	95	5F
	0x00,  //	96	60
	0x00,  //	97	61
	0x00,  //	98	62
	0x00,  //	99	63
	0x00,  //	100	64
	0x00,  //	101	65
	0x00,  //	102	66
	0x00,  //	103	67
	0x00,  //	104	68
	0x00,  //	105	69	0xE0	0x69	end						##CP/M cursor end of line
	0x00,  //	106	6A
	ZX_KEY_LEFT,  //	107	6B	0xE0	0x6B	cursor	left
	0x00,  //	108	6C	0xE0	0x6C	home					##CP/M end cursor home
	0x00,  //	109	6D
	0x00,  //	110	6E
	0x00,  //	111	6F
	0x00,  //	112	70	0xE0	0x70	insert					##CP/M /
	0x00,  //	113	71	0xE0	0x71	delete					##CP/M /
	ZX_KEY_DOWN,  //	114	72	0xE0	0x72	cursor	down
	0x00,  //	115	73
	ZX_KEY_RIGHT,  //	116	74	0xE0	0x74	cursor	right
	ZX_KEY_UP,  //	117	75	0xE0	0x75	cursor	up
	0x00,  //	118	76
	0x00,  //	119	77
	0x00,  //	120	78
	0x00,  //	121	79
	0x00,  //	122	7A	0xE0	0x7A	page	down			##CP/M 
	0x00,  //	123	7B
	0x00,  //	124	7C
	0x00  //	125	7D	0xE0	0x7D	page	up				##CP/M  
};

#define PS2_KEY_CODE_T				44
#define PS2_KEY_CODE_USR			15 //### PS2 unused code, reused for combo key USR (Ext Mode + L)
#define PS2_KEY_CODE_0				69
#define PS2_KEY_CODE_1				22 
#define PS2_KEY_CODE_2				30
#define PS2_KEY_CODE_4				37
#define PS2_KEY_CODE_6				54
#define PS2_KEY_CODE_ESC			118
#define PS2_KEY_CODE_S				27
#define PS2_KEY_CODE_V				42
#define PS2_KEY_CODE_P				77
#define PS2_KEY_CODE_I				67
#define PS2_KEY_CODE_J				59
#define PS2_KEY_CODE_STAR			124
#define PS2_KEY_CODE_D				35
#define PS2_KEY_CODE_SEMICOLON		76
#define PS2_KEY_CODE_CR				90
#define PS2_KEY_CODE_RIGHT_SHIFT	89

#define PS2_KEY_CODE_ALT			17

#define PS2_KEY_CODE_DOUBLE_QUOTE	81


#define PS2_KEY_CODE_F3  	4	//0x04	F3
#define PS2_KEY_CODE_F1  	5	//0x05	F1
#define PS2_KEY_CODE_F2  	6	//0x06	F2
#define PS2_KEY_CODE_F4  	12	//0x12	F4
#define PS2_KEY_CODE_F12  	7	//0x07	F12
#define PS2_KEY_CAPS_LOCK	88

//RANDOMIZE USR 14446

#define MACRO_MEM_EE 

#ifdef MACRO_MEM_EE 
	#define MACRO_MEM EEMEM //macros located in EEPROM
#else
	#define MACRO_MEM PROGMEM //macros located in FLASH
#endif


const MACRO_MEM uint8_t PS2_MACRO_CPM_RUN[]={PS2_KEY_CODE_T, PS2_KEY_CODE_USR, PS2_KEY_CODE_1, PS2_KEY_CODE_4, PS2_KEY_CODE_4, PS2_KEY_CODE_4, PS2_KEY_CODE_6, PS2_KEY_CODE_CR, PS2_NO_KEY};	

//LOAD *"d";1;"  
const MACRO_MEM uint8_t PS2_MACRO_LOAD_FROM_DISK[]={PS2_KEY_CODE_J, PS2_KEY_CODE_STAR, PS2_KEY_CODE_DOUBLE_QUOTE, PS2_KEY_CODE_D, PS2_KEY_CODE_DOUBLE_QUOTE, PS2_KEY_CODE_SEMICOLON, PS2_KEY_CODE_1, PS2_KEY_CODE_SEMICOLON, PS2_KEY_CODE_DOUBLE_QUOTE, PS2_NO_KEY};
/*
//ESCAPE + p
const PROGMEM uint8_t PS2_MACRO_CPM_PAPER[]={PS2_KEY_CODE_ESC, PS2_KEY_CODE_P, PS2_NO_KEY};

//ESCAPE + i
const PROGMEM uint8_t PS2_MACRO_CPM_INK[]=	{PS2_KEY_CODE_ESC, PS2_KEY_CODE_I, PS2_NO_KEY};

//S.V.P.2024
*/
const uint8_t MACRO_MEM PS2_MACRO_SVP2024[]=	{PS2_KEY_CODE_P, 
	PS2_KEY_CODE_DOUBLE_QUOTE, 
	PS2_KEY_CAPS_LOCK, 
	PS2_KEY_CODE_S,PS2_KEY_CODE_V,PS2_KEY_CODE_P,PS2_KEY_CODE_2,PS2_KEY_CODE_0,PS2_KEY_CODE_2,PS2_KEY_CODE_4,
	PS2_KEY_CAPS_LOCK,
	PS2_KEY_CODE_DOUBLE_QUOTE, PS2_KEY_CODE_CR, PS2_NO_KEY};

