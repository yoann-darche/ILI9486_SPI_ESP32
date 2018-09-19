/* *****************************************************************************
*** Base library to drive the LCD Screen Based on ILI9486 chipset thru        **
*** SPI interface (4 wires)                                                   **
*******************************************************************************/

#ifndef _ILI9486H_SPI_ESP32_
#define _ILI9486H_SPI_ESP32_

#include <SPI.h>
#include "font.h"
#include "TColor.hpp"

/* Size of the screen */
#define TFTWIDTH	320
#define TFTHEIGHT	480

/* Command list */
#define ILI9486_INVOFF	0x20
#define ILI9486_INVON	0x21
#define ILI9486_CASET	0x2A
#define ILI9486_PASET	0x2B
#define ILI9486_RAMWR	0x2C
#define ILI9486_MADCTL	0x36

#define ILI9486_VSCROLL_DEF 0x33
#define ILI9486_VSCROLL_SADDR 0x37

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

/*
	Define pins and Output Data Registers
*/

// SPI PIN and parameters :
#define TFT_SPI_SPEED  60000000

#define TFT_SCK  14
#define TFT_MISO 12
#define TFT_MOSI 13

//Control pins
#define TFT_RST   26
#define TFT_CS    15
#define TFT_DC    33   // Data/Command
#define TFT_BLK   18   // Background led control

#define swap(a, b) { int16_t t = a; a = b; b = t; }


/* ============================================================================
== VScroll Information                                                       ==
============================================================================ */
typedef struct _VScrollData {
	uint16_t VSP = 0;     // Vertical scroll position (Default=0)
	uint16_t TFA = 0;
	uint16_t VSA = 480;
	uint16_t BFA = 0;
} VScrollData;


/* ============================================================================
==  Classe definition                                                        ==
============================================================================ */
class ILI9486_SPI_ESP32
{
public:

  // defaul constructor
	ILI9486_SPI_ESP32(void);

  // default starter get the define value to start the SPI connexion
	void	begin(void);

	// tftDC    : TFT ILI9486 Data/Command pin
	// tftReset : TFT ILI9486 rest pin
	// tftCS    : TFT ILI9486 Chip select active to Low, some tft has no CS, leave it to 0
	// tftBLK   : Is the pin used to activate or desactivate the Back led of the TFT, if none let it to 0
	// spiMISO  : is actually optional (not implemented yet)
	void	begin(uint8_t spiClk, uint8_t spiMOSI, uint8_t tftDC, uint8_t tftCS,
										  uint8_t spiMISO, uint8_t tftReset, uint8_t tftBLK);

	void	setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

	void	pushColor(uint16_t color);
	void	pushColor(TColor& color);
	void    pushColorN(uint16_t color, uint32_t n);
	void    pushColorN(TColor& color,  uint32_t n);

	void	drawPixel(int16_t x, int16_t y, uint16_t color);

	void	fillScreen(TColor& color);
	void	drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1, uint16_t color);

	void	drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	void	drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void	fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

	void	setRotation(uint8_t r);
	void	invertDisplay(uint8_t i);

	void reset(void);

	// Color Management
	uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
	void setForegroundColor(uint16_t color); // mode 565
	void setBackgroundColor(uint16_t color);
	void setForegroundColor(TColor &color);  // Extended mode
	void setBackgroundColor(TColor &color);

	// Scrolling Management
	void setVerticalScrolling(uint16_t TFA, uint16_t VSA, uint16_t BFA);
	void setVScrollStart(uint16_t VSP);
	void doBotomUpScroll(uint16_t nbLine, uint8_t isClean, uint16_t tDelay);
	
	// Characater function
	void SetConsolFont(const unsigned char *Font, uint8_t sizeW, uint8_t sizeH);
	
	size_t write(const uint8_t *buffer, size_t size);
	size_t writeAt(uint8_t x, uint8_t y, const uint8_t *Text, uint8_t TextLen);

	void PrintStringAt(uint8_t x, uint8_t y, const char *Text, uint16_t TextStart, uint8_t TextLen);

	void printBottomUpScroll(const char *Text);
	

protected:

    uint16_t _height;
    uint16_t _width;


	uint8_t _spiClk;
	uint8_t _spiMISO;
	uint8_t _spiMOSI;
	uint8_t _tftReset;
	uint8_t _tftDC;
	uint8_t _tftCS;
	uint8_t _tftBLK;
	uint8_t	tabcolor;

	// Active color
	TColor foregroundColor = TColor(0xFFFF);
	TColor backgroundColor = TColor(0x0000);

	// Scrolling Data
	VScrollData VData;

	// Character variable
	uint8_t c_sizeW = 8;
	uint8_t c_sizeH = 8;
	const unsigned char * c_font = Consol_CGATHIN_8x8;



	void sendColor(uint16_t color) ;
	void writeCommand(uint8_t c);
	void writeData(uint8_t d);
	void writeData16(uint16_t d);
	void writeData16(uint16_t d, uint32_t num);
	void commandList(const uint8_t *addr);

	void CD_DATA(void);
	void CD_COMMAND(void);
	void CS_ON(void);
	void CS_OFF(void);
};







#endif
