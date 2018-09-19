#ifndef __TC_COLOR
#define __TC_COLOR

#include <stdint.h>

/*============================================================================
== Color définition                                                         ==
============================================================================*/
class TColor {

	public:
		TColor() { R=0; G=0; B=0; }
		TColor(uint16_t c565) { set565(c565); }
		TColor(uint8_t R, uint8_t G, uint8_t B) { setRGB(R,G,B); }

		void setRGB(uint8_t r, uint8_t g, uint8_t b) { R=r; G=g; B=b; }
		void set565(uint16_t color) { R=(((color>>11) & 0xb0011111) << 3) & 0xf4; B=(((color>>5)  & 0xb0111111) << 2) & 0xfc; G=((color & 0xb0011111) << 3) & 0xf4; }
		// 4bit <I1> <R1> <G1> <B1> ::  Red = <I1> <R1> <R1> <R1> <0> <0> <0> <0>
		void set4bit(uint8_t rgb) { R = (rgb & 0x04) > 0 ? (0x70 + (rgb & 0x08) * 0x80) : 0; G = (rgb & 0x02) > 0 ? (0x70 + (rgb & 0x08) * 0x80) : 0; B = (rgb & 0x01) > 0 ? (0x70 + (rgb & 0x08) * 0x80) : 0; }
		// 8bit <I2> <R2> <G2> <B2> ::  Red = <I2> <I1> <R2> <R1> <0> <0> <0> <0>
		void set8bit(uint8_t rgb) { R = rgb & 0xF0; G = (rgb & 0xC0) + ((rgb & 0x0C) << 2); B = (rgb & 0xC0) +  ((rgb & 0x03) << 4); }


		uint8_t getR() { return R; }
		uint8_t getG() { return G; }
		uint8_t getB() { return B; }

		// Output formated for ILI9486 18bit color
		uint8_t getR6() { return (R & 0xFC); }
		uint8_t getG6() { return (G & 0xFC); }
		uint8_t getB6() { return (B & 0xFC); }

		uint8_t get4bit() {return ((B >> 4) > 0 ? 1:0) + ((G >> 4) > 0 ? 2:0) +  ((R >> 4) > 0 ? 4:0) + ((R+G+B/3 > 128) ? 8 :0); }
		uint8_t get8bit() {return ( ((B >> 4) & 0x03) + ((G >> 2) & 0x0C) + (R & 0x30) + (((R+G+B)/3) & 0xC0) ) ; }
		uint16_t get565() {return ( ((R & 0xF8) << 8) + ((G & 0xFC) << 3) + ((B & 0xF4) >> 3) );}

		void copy(TColor &c) { R = c.R; G=c.G; B=c.B; }

	protected:
	  uint8_t R;
	  uint8_t G;
	  uint8_t B;
};

/*==============================================================================
* Main color def       
*=============================================================================*/
extern TColor tcBLACK;
extern TColor tcRED;
extern TColor tcCYAN;
extern TColor tcGREEN;
extern TColor tcBLUE;
extern TColor tcGRAY;
extern TColor tcYELLOW;
extern TColor tcWHITE;


#endif