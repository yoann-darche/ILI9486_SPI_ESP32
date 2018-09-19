#ifndef _TEXT_ENGINE_
#define _TEXT_ENGINE_

#include <Arduino.h>
#include "ILI9486_SPI_ESP32.h"


// Line Status Code
#define TE_LS_NONE         0x00
#define TE_LS_CONTINUE     0x01
#define TE_LS_DIRTY        0x10

#define TE_TAB_SIZE        0x05


class TextEngine : public ILI9486_SPI_ESP32
{

public:
    TextEngine();
    ~TextEngine();

    /*        
        Mode  0: 8x14 16colors 40x34 :  2720 o ( 2.7 Ko)  <C8> <B4><F4>
        Mode  1: 8x8  16colors 40x60 :  4800 o ( 4.7 Ko)  <C8> <B4><F4>
                
        Mode 16: 8x14 256      40x34 :  4080 o ( 4.0 Ko)  <C8> <B8> <F8>
        Mode 17: 8x8  256      40x60 :  7200 o ( 7.0 Ko)  <C8> <B8> <F8>              
        
        Mode 32: 8x14 65535    40x34 :  6800 o ( 6.6 Ko)  <C8> <B16> <F16>
        Mode 33: 8x8  65535    40x60 : 12000 o (11.8 Ko)  <C8> <B16> <F16>              
    */
    void setMode(uint8_t mode);

    // Direct memory access
    void     poke(uint16_t addr, uint8_t val);
    uint8_t  peek(uint16_t addr);
    void     poke16(uint16_t addr, uint16_t val);
    uint16_t peek16(uint16_t addr);

    // print familly function
    void print(const char *);

    // Text consol management
    void cls(void);
    uint8_t locate(uint8_t x, uint8_t y);

    uint8_t goNextLine(void);
    void    printString(const char *Text);
    size_t  write(const uint8_t *buffer, size_t size);

    // Dynamique update (ie cursor)
    void DoUpdate(uint32_t delay);
    
    // Editor functions
    void SetEditorMode(void);
    void SetRunMode(void);

protected:

    // Text consol management
    uint8_t  CurX = 0;
	uint8_t  CurY = 0;
    bool     CurVisible = true;
    uint16_t blinkDelay = 500;

    // Editor
    bool IsEditorMode  = false;

    // Screen memory
    char *Screen = NULL;
    uint8_t *LineStatus = NULL;   // Line Status for each line : 0x00 : nothing, 0x01 : Line continue to the next one

    uint16_t _ScreenSize = 0;

    uint8_t _mode = 0;
    uint8_t _maxX = 40;
    uint8_t _maxY = 34;
    uint8_t _colorSize = 1;  // number of octet needed to store background and foreground color (8bit or 32bit, 16bit?)


    // Gfx Utilities Functions
    void _DoUpdateScreen(uint16_t addr, uint16_t nb);
    uint16_t _getScreenFGColor(uint16_t addr);
    uint16_t _getScreenBGColor(uint16_t addr);
    void _pokeChar(uint8_t x, uint8_t y, char c);

};


#endif