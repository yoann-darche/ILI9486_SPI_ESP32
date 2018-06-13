#include <arduino.h>
#include "ILI9486_SPI_ESP32.h"
#include "font.h"

uint16_t lineBuffer[1];

/*****************************************************************************/
// Constructor uses hardware SPI, the pins being specific to each device
/*****************************************************************************/

#if defined(__ADAFRUIT_COMPATIBLE__)
  ILI9486_SPI_ESP32::ILI9486_SPI_ESP32(void) : Adafruit_GFX(TFTWIDTH, TFTHEIGHT) {}
#else
  ILI9486_SPI_ESP32::ILI9486_SPI_ESP32(void) {

    _height = TFTHEIGHT;
    _width  = TFTWIDTH;
}
#endif

/*******************************************************************************
** Boot sequence of the LCD (SPI 4 Wire and 18bit color)                      **
*******************************************************************************/
#define DELAY 0x80

const uint8_t ili9486_init_sequence[] =
{
	2, 0xb0, 0x0,	// Interface Mode Control
	1, 0x11,		// Sleep OUT
	DELAY, 150,
	2, 0x3A, 0x66,	// use 18 bits per pixel color the only mode available on SPI-3wire ou SPI-4wire
	2, 0x36, 0x48,	// MX, BGR == rotation 0
	2, 0xC2, 0x44,	// Power Control 3
	// VCOM Control 1
	5, 0xC5, 0x00, 0x00, 0x00, 0x00,
	// PGAMCTRL(Positive Gamma Control)
	16, 0xE0, 0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08, 0x48, 0x98,
	          0x37, 0x0A, 0x13, 0x04, 0x11, 0x0D, 0x00,
	// NGAMCTRL(Negative Gamma Control)
	16, 0xE1, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75,
	          0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00,
	// Digital Gamma Control 1
	16, 0xE2, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75,
	          0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00,
	1, 0x11,	// Sleep OUT
	DELAY, 150, 	// wait some time
	1, 0x29,	// Display ON
	0			// end marker
};


/******************************************************************************
** Default begin                                                             **
******************************************************************************/
void ILI9486_SPI_ESP32::begin(void)
{


	begin(TFT_SCK, TFT_MOSI, TFT_DC, TFT_CS, TFT_MISO, TFT_RST, TFT_BLK);

}

/******************************************************************************
** Parametrized Begin                                                        **
******************************************************************************/
void ILI9486_SPI_ESP32::begin(uint8_t spiClk, uint8_t spiMOSI, uint8_t tftDC, uint8_t tftCS,
										  uint8_t spiMISO, uint8_t tftReset, uint8_t tftBLK)
{

	// Store pins
	// Set Default
	_spiClk  = spiClk;
	_spiMISO = spiMISO;
	_spiMOSI = spiMOSI;
	_tftDC   = tftDC;
	_tftReset= tftReset;
	_tftCS   = tftCS;
	_tftBLK  = tftBLK;


	pinMode(tftDC, OUTPUT);

	if (_tftBLK > 0) {
		pinMode(_tftBLK, OUTPUT);
		digitalWrite(_tftBLK, LOW);
	}

	if (_tftCS > 0) {
		pinMode(_tftCS, OUTPUT);
		digitalWrite(_tftCS, HIGH);
	}

	// toggle RST low to reset
	if (_tftReset > 0) {
		pinMode(_tftReset, OUTPUT);
		digitalWrite(_tftReset, HIGH);
		delay(20);
		digitalWrite(_tftReset, LOW);
		delay(20);
		digitalWrite(_tftReset, HIGH);
		delay(200);
	}

	SPI.begin(_spiClk, _spiMISO, _spiMOSI, _tftCS);

	SPI.beginTransaction(SPISettings(TFT_SPI_SPEED, MSBFIRST, SPI_MODE0));
	commandList(ili9486_init_sequence);

	if (_tftBLK > 0) {
		digitalWrite(_tftBLK, HIGH);
	}

}


/*****************************************************************************/
void ILI9486_SPI_ESP32::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	writeCommand(ILI9486_CASET); // Column addr set
	writeData(x0 >> 8);
	writeData(x0 & 0xFF);     // XSTART
	writeData(x1 >> 8);
	writeData(x1 & 0xFF);     // XEND

	writeCommand(ILI9486_PASET); // Row addr set
	writeData(y0 >> 8);
	writeData(y0);     // YSTART
	writeData(y1 >> 8);
	writeData(y1);     // YEND

	writeCommand(ILI9486_RAMWR); // write to RAM
}

/*****************************************************************************/
void ILI9486_SPI_ESP32::pushColor(uint16_t color)
{
	CD_DATA();
	CS_ON();
		SPI.transfer( (((color>>11) & 0xb0011111) << 3) & 0xfc);
		SPI.transfer( (((color>>5)  & 0xb0111111) << 2) & 0xfc);
		SPI.transfer( ((color       & 0xb0011111) << 3) & 0xfc);
	CS_OFF();
}

void ILI9486_SPI_ESP32::pushColor(TColor& color)
{
	CD_DATA();
	CS_ON();
		SPI.transfer( color.getR6() );
		SPI.transfer( color.getG6() );
		SPI.transfer( color.getB6() );
	CS_OFF();
}

void ILI9486_SPI_ESP32::pushColorN(uint16_t color, uint32_t n)
{
	CD_DATA();
	CS_ON();
	for(uint32_t i=0; i<n;i++) {
		SPI.transfer( (((color>>11) & 0xb00011111) << 3) & 0xfc);
		SPI.transfer( (((color>>5)  & 0xb00111111) << 2) & 0xfc);
		SPI.transfer( ((color       & 0xb00011111) << 3) & 0xfc);
	}
	CS_OFF();

}

void ILI9486_SPI_ESP32::pushColorN(TColor& color,  uint32_t n) 
{
	CD_DATA();
	CS_ON();
	for(uint32_t i=0; i<n;i++) {
		SPI.transfer( color.getR6() );
		SPI.transfer( color.getG6() );
		SPI.transfer( color.getB6() );
	}
	CS_OFF();
}


/*****************************************************************************/
void ILI9486_SPI_ESP32::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

	// Correction against the scroll starting point
	y = (_VSP + y) % _height;

	setAddrWindow(x, y, x, y);
	pushColor(color);
}
/*****************************************************************************/
void ILI9486_SPI_ESP32::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	// Rudimentary clipping
	if ((x >= _width) || (y >= _height || h < 1)) return;
	if ((y + h - 1) >= _height)	{ h = _height - y; }
	if (h < 2 ) { drawPixel(x, y, color); return; }

	// Correction against the scroll starting point
	if((h == _height) || (_VSP == 0)) {

		setAddrWindow(x, y, x, y + h - 1);
		pushColorN(color,h);

	}
	else
	{
		y = (_VSP + y) % _height;

		if((y+h-1) > _height) {

			// In this case we need to draw in to part
			setAddrWindow(x, y, x, _height - 1);
			pushColorN(color,_height-y);
			setAddrWindow(x, 0, x, h-(_height-y) - 1);
			pushColorN(color,h-(_height-y));

		} else {
			setAddrWindow(x, y, x, y + h - 1);
			pushColorN(color,h);
		}
	}
	
	

	
}
/*****************************************************************************/
void ILI9486_SPI_ESP32::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	// Rudimentary clipping
	if ((x >= _width) || (y >= _height || w < 1)) return;
	if ((x + w - 1) >= _width) { w = _width - x; }
	if (w < 2 ) { drawPixel(x, y, color); return; }


	y = (_VSP + y) % _height;

	setAddrWindow(x, y, x + w - 1, y);
	pushColorN(color,w);
}

/*****************************************************************************/
void ILI9486_SPI_ESP32::fillScreen(uint16_t color)
{
	
	setAddrWindow(0, _VSP,  _width, _height);
	pushColorN(color, (_width*_height) );

	if(_VSP != 0) { setVScrollStart(0); }

}

/*****************************************************************************/
void ILI9486_SPI_ESP32::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	// rudimentary clipping (drawChar w/big text requires this)
	if ((x >= _width) || (y >= _height || h < 1 || w < 1)) return;
	if ((x + w - 1) >= _width) { w = _width  - x; }
	if ((y + h - 1) >= _height) { h = _height - y; }
	if (w == 1 && h == 1) {
		drawPixel(x, y, color);
		return;
	}

	setAddrWindow(x, y, x + w - 1, y + h - 1);
	pushColorN(color, (w*h) );
}

/*
* Draw lines faster by calculating straight sections and drawing them with fastVline and fastHline.
*/
/*****************************************************************************/
void ILI9486_SPI_ESP32::drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color)
{
	if ((y0 < 0 && y1 <0) || (y0 > _height && y1 > _height)) return;
	if ((x0 < 0 && x1 <0) || (x0 > _width && x1 > _width)) return;
	if (x0 < 0) x0 = 0;
	if (x1 < 0) x1 = 0;
	if (y0 < 0) y0 = 0;
	if (y1 < 0) y1 = 0;

	if (y0 == y1) {
		if (x1 > x0) {
			drawFastHLine(x0, y0, x1 - x0 + 1, color);
		}
		else if (x1 < x0) {
			drawFastHLine(x1, y0, x0 - x1 + 1, color);
		}
		else {
			drawPixel(x0, y0, color);
		}
		return;
	}
	else if (x0 == x1) {
		if (y1 > y0) {
			drawFastVLine(x0, y0, y1 - y0 + 1, color);
		}
		else {
			drawFastVLine(x0, y1, y0 - y1 + 1, color);
		}
		return;
	}

	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	}
	else {
		ystep = -1;
	}

	int16_t xbegin = x0;
	if (steep) {
		for (; x0 <= x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					drawFastVLine (y0, xbegin, len + 1, color);
					//writeVLine_cont_noCS_noFill(y0, xbegin, len + 1);
				}
				else {
					drawPixel(y0, x0, color);
					//writePixel_cont_noCS(y0, x0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			//writeVLine_cont_noCS_noFill(y0, xbegin, x0 - xbegin);
			drawFastVLine(y0, xbegin, x0 - xbegin, color);
		}

	}
	else {
		for (; x0 <= x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					drawFastHLine(xbegin, y0, len + 1, color);
					//writeHLine_cont_noCS_noFill(xbegin, y0, len + 1);
				}
				else {
					drawPixel(x0, y0, color);
					//writePixel_cont_noCS(x0, y0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
			//writeHLine_cont_noCS_noFill(xbegin, y0, x0 - xbegin);
			drawFastHLine(xbegin, y0, x0 - xbegin, color);
		}
	}
}



/*****************************************************************************/
// Pass 8-bit (each) R,G,B, get back 16-bit packed color
/*****************************************************************************/
uint16_t ILI9486_SPI_ESP32::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void ILI9486_SPI_ESP32::setForegroundColor(uint16_t color) {
	foregroundColor = color;
}

void ILI9486_SPI_ESP32::setBackgroundColor(uint16_t color) {
	backgroundColor = color;
}



/*****************************************************************************/
void ILI9486_SPI_ESP32::setRotation(uint8_t m)
{
	writeCommand(ILI9486_MADCTL);
	uint8_t rotation = m & 3; // can't be higher than 3
	switch (rotation) {
		case 0:
			writeData(MADCTL_MX |MADCTL_BGR);
			_width  = TFTWIDTH;
			_height = TFTHEIGHT;
			break;
		case 1:
			writeData(MADCTL_MV | MADCTL_BGR);
			_width  = TFTHEIGHT;
			_height = TFTWIDTH;
			break;
		case 2:
			writeData(MADCTL_MY | MADCTL_BGR);
			_width  = TFTWIDTH;
			_height = TFTHEIGHT;
			break;
		case 3:
			writeData(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			_width  = TFTHEIGHT;
			_height = TFTWIDTH;
			break;
	}
}

/*****************************************************************************
** Invert the display if the parameter is different from 0                  **
*****************************************************************************/
void ILI9486_SPI_ESP32::invertDisplay(uint8_t i)
{
	writeCommand(i != 0 ? ILI9486_INVON : ILI9486_INVOFF);
}


/* ============================================================================
== Vertical Scrolling management.                                            ==
============================================================================ */
void ILI9486_SPI_ESP32::setVerticalScrolling(uint16_t TFA, uint16_t VSA, uint16_t BFA) {

	writeCommand(ILI9486_VSCROLL_DEF);
	writeData16(TFA);
	writeData16(VSA);
	writeData16(BFA);
}


void ILI9486_SPI_ESP32::setVScrollStart(uint16_t VSP) {	
	writeCommand(ILI9486_VSCROLL_SADDR);
	writeData16(VSP);
	_VSP = VSP;
}

/* ============================================================================
== Peform a	vertical scroll (adding nbLine at the bottom, and scoll up).     ==
== if delay > 0 then add delay between each line added(smoth scroll delay=26)==
== if isClean <> 0 then the added line will be fill with backgroundColor     ==
============================================================================ */
void ILI9486_SPI_ESP32::doBotomUpScroll(uint16_t nbLine, uint8_t isClean, uint16_t tDelay) {

	uint16_t tVSP;

	if(delay == 0) {
		tVSP = (_VSP + nbLine) % _height;
		setVScrollStart(tVSP);
	} else {

		for(int i=0; i < nbLine; i++) {
			tVSP = (_VSP+1) % _height;
			setVScrollStart(tVSP);
			if(isClean != 0) drawFastHLine(0,_height-1,_width,backgroundColor);
			delay(tDelay);
		}

	}
}

void ILI9486_SPI_ESP32::printBottomUpScroll(const char *Text) {

	int i;
	uint16_t tVSP, y;

	uint16_t tLen = strlen(Text);

	for(int line=0; line < c_sizeH; line++) {

		tVSP = (_VSP+1) % _height;
		setVScrollStart(tVSP);

		y = (_VSP + (_height-1)) % _height;

		setAddrWindow(0,y, _width-1,y);	

		CD_DATA();
		CS_ON();
	

		for(i=0; i<tLen;i++) {

			char c = Text[i];

			unsigned char code = pgm_read_byte(c_font + c + (line * 256));

			for(int p=0;p<8;p++) {

				if(((code >> 7-p) & 0x01) == 0) {
					// Draw a backgroundColor
					SPI.transfer( (((backgroundColor>>11) & 0xb00011111) << 3) & 0xfc);
					SPI.transfer( (((backgroundColor>>5)  & 0xb00111111) << 2) & 0xfc);
					SPI.transfer( ((backgroundColor       & 0xb00011111) << 3) & 0xfc);
				}
				else {
					// Draw a white one
					SPI.transfer( (((foregroundColor>>11) & 0xb00011111) << 3) & 0xfc);
					SPI.transfer( (((foregroundColor>>5)  & 0xb00111111) << 2) & 0xfc);
					SPI.transfer( ((foregroundColor       & 0xb00011111) << 3) & 0xfc);
				}

			}
		
		}

		for(i=(tLen*8)-1;i<_width-1;i++) {
			// Draw a backgroundColor
			SPI.transfer( (((backgroundColor>>11) & 0xb00011111) << 3) & 0xfc);
			SPI.transfer( (((backgroundColor>>5)  & 0xb00111111) << 2) & 0xfc);
			SPI.transfer( ((backgroundColor       & 0xb00011111) << 3) & 0xfc);
		}


		CS_OFF();		

	}
	


}


/* ============================================================================
== Character utilities                                                       ==
=============================================================================*/

/******************************************************************************
 * Define the font familly to use for the consol.                            **
 * only support sizeW = 8                                                    **
 * ***************************************************************************/
void ILI9486_SPI_ESP32::SetConsolFont(const unsigned char *Font, uint8_t sizeW, uint8_t sizeH) {

	c_font = Font;
	c_sizeW = 8;
	c_sizeH = sizeH;
}

/******************************************************************************
 * Locate the print cursor x,y.  (0,0) is the first line/first column        **
 * Returen 0 if OK if outside the screen return 1.                           **
 *****************************************************************************/
uint8_t ILI9486_SPI_ESP32::Locate(uint8_t x, uint8_t y)
{
	if ( (x < (_width / c_sizeW)) && (y < (_height / c_sizeH)) ) {

		c_x = x;
		c_y = y;

		return 0;
	}

	return 1;
}

/******************************************************************************
 * Print the char array at the active cursor position.                       **
 * 		\n is interpreted as new line, first column.                         **
 *  	\t is select the location of the next tabstop every 5 charcarter     **
 *****************************************************************************/
void ILI9486_SPI_ESP32::PrintChar(const char *Text) {

	int Len = strlen(Text);

	uint8_t maxY = (_height / c_sizeH);
	uint8_t maxX = (_width / c_sizeW);

	uint16_t pos = 0;
	while (Len > 0) {

		// We can draw all characters
		if(c_x + Len <= maxX) {
			PrintCharAt(c_x,c_y,Text,pos,-1);
			c_x = c_x + Len;
			if (c_y >= maxY) c_y=maxY-1;
			Len = 0;
			continue;
		}

		// Get the maximum of character that can be print from the actual position
		uint8_t nbChar = maxX - c_x;
		PrintCharAt(c_x,c_y,Text,pos,nbChar);
		c_x =0;
		if (c_y >= maxY) c_y=maxY-1;
		pos += nbChar;
		Len -= nbChar;

		// go the next line
		c_y++;
	}

}

/******************************************************************************
 * Print the char array at the x,y text position, begin at the TextStart for **
 * a len character.                                                          **
 *  - if the len overlaod the screen size it will be ignored.                **
 *  - if x is outside it will be ignored.                                    **
 *  - if y is outside it will be print at the last line with a V-scroll      **
 *  - if TextLen = 255, all character will be printed.                       **
 *****************************************************************************/
void ILI9486_SPI_ESP32::PrintCharAt(uint8_t x, uint8_t y, const char *Text, uint16_t TextStart, uint8_t TextLen) {


	// Check if the logical position is fine
	if(x >= (_width/c_sizeW))  return;

	uint8_t doScroll = (y >= (_height/c_sizeH)) ? 1 :0;

	y = (doScroll == 1) ? _height/c_sizeH-1 : y;

	uint16_t px = x*c_sizeW;
	uint16_t py = (_VSP + (y * c_sizeH)) % _height;
	
	// Check that text start is in coherence with the Text array
	uint16_t _lenText = strlen(Text);
	if(TextStart > _lenText) return;
	if( (TextStart+TextLen) > _lenText) TextLen = _lenText - TextStart;

	// Calculate the real number of character to display accordind the x,y position
	uint8_t nbCar =  (_width / c_sizeW) - x < TextLen ? (_width / c_sizeW) - x : TextLen;

	// for each line of the character font
	for(int line=0; line < c_sizeH; line++) {

		if(doScroll == 1) {

			uint16_t tVSP = (_VSP+1) % _height;
			setVScrollStart(tVSP);
			py = (_VSP + (_height-1)) % _height;
			setAddrWindow(0,py, _width-1,py);	
		}
		else
		{
			setAddrWindow(px,py+line, px+nbCar*c_sizeW,py+line);
		}

		CD_DATA();
		CS_ON();


		for(int i=0; i<nbCar;i++) {

			char c = Text[i+TextStart];

			unsigned char code = pgm_read_byte(c_font + c + (line * 256));

			for(int p=0;p<8;p++) {

				if(((code >> 7-p) & 0x01) == 0) {
					// Draw a backgroundColor
					SPI.transfer( (((backgroundColor>>11) & 0xb00011111) << 3) & 0xfc);
					SPI.transfer( (((backgroundColor>>5)  & 0xb00111111) << 2) & 0xfc);
					SPI.transfer( ((backgroundColor       & 0xb00011111) << 3) & 0xfc);
				}
				else {
					// Draw a white one
					SPI.transfer( (((foregroundColor>>11) & 0xb00011111) << 3) & 0xfc);
					SPI.transfer( (((foregroundColor>>5)  & 0xb00111111) << 2) & 0xfc);
					SPI.transfer( ((foregroundColor       & 0xb00011111) << 3) & 0xfc);
				}

			}
		
		}

		if(doScroll == 1) {
			for(int i=(nbCar*8)-1;i<_width-1;i++) {
				// Draw a backgroundColor
				SPI.transfer( (((backgroundColor>>11) & 0xb00011111) << 3) & 0xfc);
				SPI.transfer( (((backgroundColor>>5)  & 0xb00111111) << 2) & 0xfc);
				SPI.transfer( ((backgroundColor       & 0xb00011111) << 3) & 0xfc);
			}
		}

		CS_OFF();

						
	}

	if(doScroll == 1) {
		for(int i=0;i< (_height % c_sizeH);i++) {
			uint16_t tVSP = (_VSP+1) % _height;
			setVScrollStart(tVSP);
			py = (_VSP + (_height-1)) % _height;
			setAddrWindow(0,py, _width-1,py);
			CD_DATA();
			CS_ON();
			for(int j=0;j<_width-1;j++) {
				// Draw a backgroundColor
				/*SPI.transfer( (((backgroundColor>>11) & 0xb00011111) << 3) & 0xfc);
				SPI.transfer( (((backgroundColor>>5)  & 0xb00111111) << 2) & 0xfc);
				SPI.transfer( ((backgroundColor       & 0xb00011111) << 3) & 0xfc);*/

				SPI.transfer( 64);
				SPI.transfer( 128);
				SPI.transfer( 64);
			}
			CS_OFF();
		}
	}

}

/* ============================================================================
== UTILITIES                                                                 ==
=============================================================================*/

/*****************************************************************************/
// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
/*****************************************************************************/
void ILI9486_SPI_ESP32::commandList(const uint8_t *addr)
{
	uint8_t  numBytes, tmp;

	while ( (numBytes=(*addr++))>0 ) { // end marker == 0
		if ( numBytes&DELAY ) {
			tmp = *addr++;
			delay(tmp); // up to 255 millis
		} else {
			tmp = *addr++;
			writeCommand(tmp); // first byte is command
			while (--numBytes) { //   For each argument...
				tmp = *addr++;
				writeData(tmp); // all consecutive bytes are data
			}
		}
	}
}

// 	_tftDC _tftReset _tftCS	_tftBLK

void ILI9486_SPI_ESP32::CD_DATA(void) {
	gpio_set_level((gpio_num_t)_tftDC, 1);
}

void ILI9486_SPI_ESP32::CD_COMMAND(void) {
	gpio_set_level((gpio_num_t)_tftDC, 0);
}

void ILI9486_SPI_ESP32::CS_ON(void) {
	if(_tftCS > 0) digitalWrite(_tftCS, LOW);
}

void ILI9486_SPI_ESP32::CS_OFF(void) {
	if(_tftCS > 0) digitalWrite(_tftCS, HIGH);
}


/*****************************************************************************/
void ILI9486_SPI_ESP32::writeData(uint8_t c)
{
	CD_DATA();
	CS_ON();
    SPI.transfer(c);
	CS_OFF();
}

void ILI9486_SPI_ESP32::writeData16(uint16_t c)
{
	CD_DATA();
	CS_ON();
	SPI.transfer(c>>8);
	SPI.transfer(c&0xFF);
	CS_OFF();
}

void ILI9486_SPI_ESP32::writeData16(uint16_t color, uint32_t num)
{
	CD_DATA();
	CS_ON();
	for (int n=0;n<num;n++) {
		SPI.transfer(color>>8);
		SPI.transfer(color&0xFF);
	}
	CS_OFF();
}

void ILI9486_SPI_ESP32::writeCommand(uint8_t c)
{
	CD_COMMAND();
	CS_ON();
    SPI.transfer(c);
	CS_OFF();
}
