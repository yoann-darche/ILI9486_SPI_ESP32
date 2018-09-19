#include "TextEngine.h"


/******************************************************************************
 * Constructor / Destructor                                                  **
 * ************************************************************************* */
TextEngine::TextEngine() {

    setMode(0);

}

TextEngine::~TextEngine() {

    if(Screen != NULL) free(Screen);
    Screen = NULL;
    if(LineStatus != NULL) free(LineStatus);
    LineStatus = NULL;
}


/*****************************************************************************
 * Setting functions.                                                       **
 * ************************************************************************ */
void TextEngine::setMode(uint8_t mode) {

    _mode = mode;

    // Select the font 8x8 or 8x14
    switch(_mode & 0x0F) {
        case 0x00:
            {
                SetConsolFont(Consol_VGA_8x14,8,14);
                setVerticalScrolling(0,476,4);
                _maxX = 40;
                _maxY = 34;
                break;
            }
        case 0x01: 
            {
                SetConsolFont(Consol_CGATHIN_8x8,8,8);
                setVerticalScrolling(0,480,0);
                _maxX = 40;
                _maxY = 60; 
                break;
            }
        default:
            {
                SetConsolFont(Consol_VGA_8x14,8,14);
                setVerticalScrolling(0,476,4);
                _maxX = 40;
                _maxY = 34;
                break;
            } 
    }

    // Select the color deph (2x4bit, 2x8bit or 2x16bit)
    switch(_mode & 0xF0) {
        case 0x00: _colorSize = 1; break;
        case 0x10: _colorSize = 2; break;
        case 0x20: _colorSize = 4; break;
        default: _colorSize = 1;
    }

    // Allocation du buffeur
    if (Screen != NULL) free(Screen);
    if (LineStatus != NULL) free(LineStatus);

    _ScreenSize = _maxX * _maxY * (_colorSize + 1);
    Screen = (char *)malloc(_ScreenSize);
    LineStatus = (uint8_t *)malloc(_maxY);

   // Serial.printf("Set _ScreenSize=%d ::: _maxX=%d _maxY=%d _colorSize=%d",_ScreenSize, _maxX, _maxY, _colorSize);

    fillScreen(tcBLACK);
   
}

/* ============================================================================
== Print utilities                                                           ==
============================================================================ */

void TextEngine::print(const char*Str) {

    uint16_t L = strlen(Str);

    for(int i=0; i < L; i++) {

        switch(Str[i]) {

            case '\n':

                break;
            case '\t':

                break;

            default:
                break;


        }

    }


}

/* ============================================================================
== Editor Functions                                                          ==
============================================================================ */
void TextEngine::SetEditorMode(void) {

    if(IsEditorMode == true) return;

    IsEditorMode =  true;
    // Show cursor

}
    
void TextEngine::SetRunMode(void) {

    if(IsEditorMode == false) return;

    IsEditorMode =  false;
    // Hide cursor
    
}


/* ============================================================================
== Store a char in memory with its color based on internal color value.      ==
============================================================================ */
void TextEngine::_pokeChar(uint8_t x, uint8_t y, char c) {


    //Check
    if ((x > _maxX) || (y > _maxY)) return;

    // Calculate the adress
    uint16_t p = x + y * _maxX * (_colorSize + 1);

    Screen[p] = c;

    switch(_colorSize) {
        case 1: Screen[p+1] = (backgroundColor.get4bit() << 4) + foregroundColor.get4bit(); break;
        case 2: Screen[p+1] = backgroundColor.get8bit(); Screen[p+2] = foregroundColor.get8bit(); break;
        case 4: 
            {
                uint16_t bg = backgroundColor.get565();
                uint16_t fg = foregroundColor.get565();
             
                Screen[p+1] = (bg >> 8) & 0xFF;
                Screen[p+2] = bg & 0xFF;
                Screen[p+3] = (fg >> 8) & 0xFF;
                Screen[p+4] = fg & 0xFF;
                break;
            }
        default: ;
    }


}


/* ============================================================================
== Draw / ReDraw Screen utility.                                             ==
============================================================================ */
uint16_t TextEngine::_getScreenFGColor(uint16_t addr) {

    uint8_t c;
    uint16_t r=0;

    switch(_colorSize) {
        case 1: 
            {
                c = Screen[addr+1];
                // Intensity (c & 0x08)
                    // R
                if((c & 0x0F) == 0x08) { r= 0x528A;}
                else
                {  
                r = ( (c & 0x04) > 0 ? (0x7800 + ( (c & 0x08) > 0 ? 0x8000 : 0x0000)) : 0x0000) +
                    // V
                    ( (c & 0x02) > 0 ? (0x03E0 + ( (c & 0x08) > 0 ? 0x0400 : 0x0000)) : 0x0000) +
                    // B
                     ( (c & 0x01) > 0 ? (0x000F + ( (c & 0x08) > 0 ? 0x0010 : 0x0000)) : 0x0000);
                }
                                    
             //   Serial.printf("C> %d => %X \n",c,r);
                return (r);
                break;
            }
        case 2: 
            {
                c = Screen[addr+2];
                uint8_t I = (c >> 6) & 0x03;
                uint16_t r=0;

                if ( (c & 0x30) > 0) {

                    r = ( (c & 0x20) > 0 ? 0x2000 : 0x0000) + ((c & 0x10) > 0 ? 0x1800 : 0x0000);
                    r += I << 14;
                }

                if ( (c & 0x06) > 0) {
                    r += ( (c & 0x08) > 0 ? 0x0180 : 0x0000) + ( (c & 0x04) > 0 ? 0x0060 : 0x0000);
                    r += I << 10;
                }

                if ( (c & 0x03) > 0) {
                    r +=  ( (c & 0x02) > 0 ? 0x0004 : 0x0000) + ( (c & 0x01) > 0 ? 0x0003 : 0x0000);
                    r += I << 3;
                }
                return (r);
                break;
            }
        case 4: 
            {
                return ( (Screen[addr+3] << 8) + Screen[addr+4]);
                break;
            }
        default:
            return 0xFFFF;  
    }    

}

uint16_t TextEngine::_getScreenBGColor(uint16_t addr) {

    uint8_t c;    
    uint16_t r=0;
    

    switch(_colorSize) {
        case 1: 
            {
                c = Screen[addr+1];
                // Intensity (c & 0x08)
                // R
                if((c & 0xF0) == 0x80) { r= 0x528A;}
                else
                {  
                r = ( (c & 0x40) > 0 ? (0x7800 + ( (c & 0x80) > 0 ? 0x8000 : 0x0000)) : 0x0000) +
                    // V
                    ( (c & 0x20) > 0 ? (0x03E0 + ( (c & 0x80) > 0 ? 0x0400 : 0x0000)) : 0x0000) +
                    // B
                     ( (c & 0x10) > 0 ? (0x000F + ( (c & 0x80) > 0 ? 0x0010 : 0x0000)) : 0x0000);
                }
                return(r);
                break;
            }
        case 2: 
            {
                c = Screen[addr+1];
                uint8_t I = (c >> 6) & 0x03;

                if ((c & 0x30) > 0) {

                    r = ( (c & 0x20) > 0 ? 0x2000 : 0x0000) + ((c & 0x10) > 0 ? 0x1800 : 0x0000);
                    r += I << 14;
                }

                if ( (c & 0x06) > 0) {
                    r += ( (c & 0x08) > 0 ? 0x0180 : 0x0000) + ( (c & 0x04) > 0 ? 0x0060 : 0x0000);
                    r += I << 10;
                }

                if ( (c & 0x03) > 0) {
                    r +=  ( (c & 0x02) > 0 ? 0x0004 : 0x0000) + ( (c & 0x01) > 0 ? 0x0003 : 0x0000);
                    r += I << 3;
                }
                return (r);
                break;
            }
        case 4: 
            {
                return ( (Screen[addr+1] << 8) + Screen[addr+2]);
                break;
            }
        default:
            return 0x0000;  
    }    

}


void TextEngine::_DoUpdateScreen(uint16_t addr, uint16_t nb) {

    // Buffer (max_size of line)
    char tmpBuf[40];
    uint8_t nbCar=0;

    // Get the addr of the slice <car><color..>
    uint16_t syncPos = addr / (_colorSize+1);
    // Calculate the corresponding c_x, c_y
    uint8_t lcy = syncPos / _maxX;
    uint8_t lcx = syncPos % _maxX;


    uint16_t syncOffset = 0;  // Store offset of the already processed character
    uint8_t jcx=lcx;
    uint8_t jcy=lcy;
    tmpBuf[0] = 0;
    nbCar =0;

    //Serial.printf(">>> lcx=%d, lcy=%d, syncPos=%d, nb=%d\n", lcx, lcy, syncPos, nb);

    for(uint16_t cpt=0; cpt < nb; cpt++) {

        if(jcx < _maxX) {

            tmpBuf[nbCar] = Screen[(jcy*_maxX+jcx) * (_colorSize+1)];
            nbCar++;
            jcx++;
           // Serial.print("..A");

        } else {

            // We are at the max position of line
            // Go to draw
            //Serial.println("\nGoToDraw !!!");
            uint16_t px = lcx*c_sizeW;
	        uint16_t py = (VData.VSP + (jcy * c_sizeH)) % VData.VSA + VData.TFA;

            for(int line=0; line < c_sizeH; line++) {

                setAddrWindow(px,py+line, px+nbCar*c_sizeW,py+line);


                CD_DATA();
                CS_ON();
                for(int i=0; i<nbCar;i++) {

                    char c = tmpBuf[i];

                    unsigned char code = pgm_read_byte(c_font + c + (line * 256));

                    uint16_t bg = _getScreenBGColor( (syncPos+syncOffset+i) * (_colorSize+1) );
                    uint16_t fg = _getScreenFGColor( (syncPos+syncOffset+i) * (_colorSize+1) );


                    for(int p=0;p<8;p++) {

                        if(((code >> (7-p)) & 0x01) == 0) {
                            // Draw a backgroundColor
                            sendColor(bg);
                        }
                        else {
                            // Draw a white one
                            sendColor(fg);
                        }
                    }                
                }
                CS_OFF();

            }

            // Processing the next block (ligne)
            jcx = 0; lcx = 0;
            jcy++;
            syncOffset += nbCar;
            nbCar = 0;

        }
    }
    //Serial.println("\nEND 0001 !!!");
    // Cas où nbCar > 0
    if(nbCar > 0) {

            //Serial.println("\nDrawBis !!!");
            uint16_t px = lcx*c_sizeW;
	        uint16_t py = (VData.VSP + (jcy * c_sizeH)) % VData.VSA + VData.TFA;

            //Serial.printf(">>>> px=%d, py=%d, nbCar=%d\n",px,py,nbCar);

            for(int line=0; line < c_sizeH; line++) {

                setAddrWindow(px,py+line, px+nbCar*c_sizeW,py+line);


                CD_DATA();
                CS_ON();
                for(int i=0; i<nbCar;i++) {

                    char c = tmpBuf[i];

                    unsigned char code = pgm_read_byte(c_font + c + (line * 256));

                    uint16_t bg = _getScreenBGColor( (syncPos+syncOffset+i) * (_colorSize+1) );
                    uint16_t fg = _getScreenFGColor( (syncPos+syncOffset+i) * (_colorSize+1) );

                    //Serial.printf(">>>>>>> Color %d, %d\n",fg,bg);


                    for(int p=0;p<8;p++) {

                        if(((code >> (7-p)) & 0x01) == 0) {
                            // Draw a backgroundColor
                            sendColor(bg);
                        }
                        else {
                            // Draw a white one
                            sendColor(fg);
                        }
                    }                
                }
                CS_OFF();

            }
    }

}


/* ============================================================================
== Cursor Management                                                         ==
============================================================================ */

/******************************************************************************
 * Clear Screen.                                                             **
 * ***************************************************************************/
void TextEngine::cls(void) {

    fillScreen(backgroundColor);

    CurX = 0;
	CurY = 0;
}

/******************************************************************************
 * Locate the print cursor x,y.  (0,0) is the first line/first column        **
 * Returen 0 if OK if outside the screen return 1.                           **
 *****************************************************************************/
uint8_t TextEngine::locate(uint8_t x, uint8_t y) {

	if ( (x < (_width / c_sizeW)) && (y < (_height / c_sizeH)) ) {

		CurX = x;
		CurY = y;

		return(0);
	}

	return(1);    
}

/******************************************************************************
 * Function that performe a cariage return on the screen. if we are at the    *
 * bottom of the screen an automated scroll up is générated.                  *
 * return 1 if a scroll up has been done otherwise 0.                         *
 *****************************************************************************/
uint8_t TextEngine::goNextLine(void)
{

	CurX=0;

	if (CurY +1  >= (_height / c_sizeH) ) {

		doBotomUpScroll(c_sizeH, 1,0);
		return(1);
	}

	CurY++;
	return(0);
}

/******************************************************************************
 * Print the char array at the active cursor position.                       **
 *****************************************************************************/
void TextEngine::printString(const char *Text) {

	int Len = strlen(Text);

	uint8_t maxY = (_height / c_sizeH);
	uint8_t maxX = (_width / c_sizeW);

	uint16_t pos = 0;
	while (Len > 0) {

		// We can draw all characters
		if(CurX + Len <= maxX) {
			PrintStringAt(CurX,CurY,Text,pos,-1);
			CurX = CurX + Len;
			if (CurY >= maxY) CurY=maxY-1;
			Len = 0;
			continue;
		}

		// Get the maximum of character that can be print from the actual position
		uint8_t nbChar = maxX - CurX;
		PrintStringAt(CurX,CurY,Text,pos,nbChar);
		CurX =0;
		if (CurY >= maxY) CurY=maxY-1;
		pos += nbChar;
		Len -= nbChar;

		// go the next line
		CurY++;
	}

}

/******************************************************************************
 * Print the buffer directly to the screen (like PrintString)                 *
 * ***************************************************************************/
size_t TextEngine::write(const uint8_t *buffer, size_t size) {

	uint8_t maxY = (_height / c_sizeH);
	uint8_t maxX = (_width / c_sizeW);
	uint16_t pos = 0;

	size_t temp = size;


	while (size > 0) {

		// We can draw all characters
		if(CurX + size <= maxX) {
			writeAt(CurX,CurY, buffer, size);
			CurX = CurX + size;
			if (CurY >= maxY) CurY=maxY-1;
			size = 0;
			continue;
		}

		// Get the maximum of character that can be print from the actual position
		uint8_t nbChar = maxX - CurX;
		writeAt(CurX,CurY, buffer+pos, nbChar);
		
		CurX =0;
		if (CurY >= maxY) CurY=maxY-1;
		pos += nbChar;
		size -= nbChar;

		// go the next line
		CurY++;
	}

	return(temp);

}


/* ============================================================================
== Cursor Blinking Mgt                                                       ==
============================================================================ */
void TextEngine::DoUpdate(uint32_t delay) {

    // Si le delais est supérieur au temps de clignottement changement état
    if(delay > blinkDelay) { CurVisible = not CurVisible; }
}

/* ============================================================================
==               Direct Memory Access Functions                              ==
============================================================================ */

void TextEngine::poke(uint16_t addr, uint8_t val) {


    Serial.printf("Poke IN :[%d]=%d :: ScreenSize = %d ::>>",addr,val,_ScreenSize);
    if(addr >= _ScreenSize) return;

    Screen[addr] = val;
    Serial.println("Poke >> _DoUpdateScreen");
    _DoUpdateScreen(addr,1);    
}

uint8_t TextEngine::peek(uint16_t addr) {

    if(addr >= _ScreenSize) return 0xFF;  // Not so good !

    return(Screen[addr]);
}