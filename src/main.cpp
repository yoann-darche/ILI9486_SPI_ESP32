#include <Arduino.h>

#include <ILI9486_SPI_ESP32.h>
#include "font.h"


#define TSPI_SCK  19
#define TSPI_MISO 0   // Ignore
#define TSPI_MOSI 13

//Control pins
#define TTFT_RST  26
#define TTFT_CS   0    // Ignore
#define TTFT_DC   15   // Data/Command
#define TFT_BLK   18   // LED


ILI9486_SPI_ESP32 tft;


void drawATest() {

    int i=0;

    tft.fillScreen(0);

    for (i=0;i<320;i+=10) {
        tft.drawLine(i, 0,0, 480, i*100+1000+4*i);
    }
    


}

void setup() {
    // put your setup code here, to run once:

   	Serial.begin(115200);
    delay(10);

	Serial.println("***** ILI9486 graphic Test V2 *****");

	tft.begin(TSPI_SCK,TSPI_MOSI,TTFT_DC,TTFT_CS,TSPI_MISO,TTFT_RST,TFT_BLK);

    tft.setRotation(0);

    tft.SetConsolFont(Consol_VGA_8x14,8,14);

    tft.setVerticalScrolling(0,476,4);  // For 8x14, with have 4 lines

}

void loop() {
    // put your main code here, to run repeatedly:
    int i,k,l;

    Serial.println("Efface l'écran...");
    tft.fillScreen(0);

    drawATest();


    
    
    /*
    for(l=1;l<6;l++) {
        for(k=0;k<10;k++) {

            tft.doBotomUpScroll(10,1,l*10);

            tft.drawFastVLine(5*(k+l*10),100,250,0xFEEF);

            delay(1000);
        }
    }
    */
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
    for(i=0;i<256;i++) {
        t[0]=i;
        tft.PrintChar(t);
    }

    delay(5000);


}
