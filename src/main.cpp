#include <Arduino.h>

#include <ILI9486_SPI_ESP32.h>
#include <TextEngine.h>
#include "font.h"


#define TSPI_SCK  19
#define TSPI_MISO 0   // Ignore
#define TSPI_MOSI 13

//Control pins
#define TTFT_RST  26
#define TTFT_CS   0    // Ignore
#define TTFT_DC   15   // Data/Command
#define TFT_BLK   18   // LED


TextEngine tft;


void drawATest() {

    int i=0;
    TColor c(0,0,0);

    tft.fillScreen(0);

    for (i=0;i<320;i+=10) {
        c.setRGB(i%64,i%255,i%255);
        tft.drawLine(i, 0,0, 480, c.get565());
    }
    


}


void Test1() {

    TColor myColor(0,0,255);

    int i,l;

    Serial.println("Test1 : Driver ILI9486");

    tft.SetConsolFont(Consol_VGA_8x14,8,14);

    tft.setVerticalScrolling(0,476,4);  // For 8x14, with have 4 lines

    tft.fillScreen(0);

    drawATest();

    tft.printBottomUpScroll("Hello ceci est un premier test"); 
    tft.printBottomUpScroll("Une deuxième ligne"); 
    tft.printBottomUpScroll("Et pourquoi pas essayer encore"); delay(1000);
    tft.printBottomUpScroll("ÿ™š³´"); delay(1000);
    tft.printBottomUpScroll("°±²²°±"); delay(1000);    


    tft.printBottomUpScroll("+------------------+");
    tft.printBottomUpScroll("|Hello new design !|");
    tft.printBottomUpScroll("|Il est important  |");
    tft.printBottomUpScroll("|de tester le text |");
    tft.printBottomUpScroll("|afin de s'assurer |");
    tft.printBottomUpScroll("|que cela fonction-|");
    tft.printBottomUpScroll("|ne bien.          |");
    tft.printBottomUpScroll("+------------------+");

    delay(7000);

    tft.printBottomUpScroll(">");
    tft.printBottomUpScroll("> Start of the LOCATE/PRINT Test");

    tft.setForegroundColor(0x9955);

    for(i=0; i<100;i++) {

        tft.setForegroundColor(random(65535));
        tft.setBackgroundColor(random(65535));

        tft.PrintCharAt(random(40),random(34),"Youhoo!",0,255);

    }

    tft.setForegroundColor(0xFFFF);
    tft.setBackgroundColor(0x0000);
    tft.printBottomUpScroll("> Done");
    delay(7000);

    tft.fillScreen(0);
    tft.printBottomUpScroll(">");
    tft.printBottomUpScroll("> Start of the PrintChar Test");
    delay(3000);

    tft.Locate(0,0);
    tft.setForegroundColor(0x9999);
    for(i=0; i<400;i++) {
        tft.PrintChar("0123456789.");
    }

    delay(10000);

    tft.fillScreen(0);
    tft.printBottomUpScroll("> Start of the LOCATE and PrintChar");
    delay(3000);

    for(i=0; i<50; i++) {
        tft.Locate(random(39),random(33));
        for(int j=0; j<30;j++) {
            tft.setForegroundColor(random(65535));
            tft.PrintChar("[^_^]");
        }
        delay(200);
    }

    delay(5000);

    tft.fillScreen(0);
    tft.Locate(0,10);
    char t[]="1";
    for(i=0;i<10;i++) {
        t[0]=i;
        tft.PrintChar(t);
    }
    delay(5000);

    tft.fillScreen(0x001F);
    tft.setForegroundColor(0xFFFF);
    tft.printBottomUpScroll("> Start Color Test RGB");

    tft.Locate(0,3);
    for(int r=0; r<255; r+=8) {
        myColor.setRGB(r,0,0);
        tft.setForegroundColor(myColor);
        tft.PrintChar("*");
    }

    tft.Locate(0,4);
    for(int g=0; g<255; g+=8) {
        myColor.setRGB(0,g,0);
        tft.setForegroundColor(myColor);
        tft.PrintChar("*");
    }

    tft.Locate(0,5);
    for(int b=0; b<255; b+=8) {
        myColor.setRGB(0,0,b);
        tft.setForegroundColor(myColor);
        tft.PrintChar("*");
    }

    int y=5;
    for(int r=0; r<255; r+=16) {

         tft.Locate(0,++y);
         for(int b=0; b<255; b+=16) {
             myColor.setRGB(r,0,b);
             tft.setForegroundColor(myColor);
             tft.PrintChar("#");
         }
    }


    delay(5000);
}

void Test2() {

    int i;
    char t[]="1";

    Serial.println("Test2 : Driver TextEngine");

    tft.setMode(0);
    tft.Locate(0,0);
    tft.PrintChar("Hello Test Mode 0");
    tft.Locate(0,1);
    tft.PrintChar("=================");

    tft.Locate(0,10);
    
    for(i=0;i<256;i++) {
        t[0]=i;
        tft.PrintChar(t);
    }

    tft.Locate(0,2);
    tft.PrintChar("Test Poke");

    for(int j=0; j< 16; j++) {

        for(i=0;i<40;i++) {

            tft.poke(i*2+j*80,254);
            tft.poke(i*2+1+j*80,j);

        }
    }
    
    delay(10000);

    tft.setMode(1);
    tft.Locate(0,0);
    tft.PrintChar("Hello Test Mode 1");
    tft.Locate(0,1);
    tft.PrintChar("=================");

    tft.Locate(0,10);
    for(i=0;i<256;i++) {
        t[0]=i;
        tft.PrintChar(t);
    }

    tft.Locate(0,2);
    tft.PrintChar("Test Poke");
    for(i=0;i<256;i++) {
        tft.poke(i*2,i);
        tft.poke(i*2+1,i);
    }

    delay(5000);

}

void Test3() {


 int i, j;
for(int j=0; j< 16; j++) {

        for(i=0;i<40;i++) {

            tft.poke(i*2+j*80,254);
            tft.poke(i*2+1+j*80,j);
            delay(5000);
        }
        delay(5000);
    }

}


void setup() {
    // put your setup code here, to run once:

   	Serial.begin(115200);
    delay(10);

	Serial.println("***** ILI9486 graphic Test V2 *****");

	tft.begin(TSPI_SCK,TSPI_MOSI,TTFT_DC,TTFT_CS,TSPI_MISO,TTFT_RST,TFT_BLK);

    tft.setRotation(0);

}

void loop() {
   
    Test1();

    Test2();

 //   Test1();


}
