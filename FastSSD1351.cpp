

#include <Adafruit_GFX.h>
#include "FastSSD1351.h"
#include "glcdfont.c"
#include "digit7segments.h"

#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

#ifndef _BV
    #define _BV(bit) (1<<(bit))
#endif

/********************************** low level pin interface */

inline void FastSSD1351::spiwrite(uint8_t c) {
    
    //Serial.println(c, HEX);
    
    if (!_sid) {
        SPI.transfer(c);
	// might be able to make this even faster but
	// a delay -is- required
	delayMicroseconds(1);
        return;
    }
    
    int8_t i;
    
    *sclkport |= sclkpinmask;
    
    for (i=7; i>=0; i--) {
        *sclkport &= ~sclkpinmask;
        //SCLK_PORT &= ~_BV(SCLK);
        
        if (c & _BV(i)) {
            *sidport |= sidpinmask;
            //digitalWrite(_sid, HIGH);
            //SID_PORT |= _BV(SID);
        } else {
            *sidport &= ~sidpinmask;
            //digitalWrite(_sid, LOW);
            //SID_PORT &= ~_BV(SID);
        }
        
        *sclkport |= sclkpinmask;
        //SCLK_PORT |= _BV(SCLK);
    }
}


void FastSSD1351::writeCommand(uint8_t c) {
    *rsport &= ~ rspinmask;
    //digitalWrite(_rs, LOW);
    
    *csport &= ~ cspinmask;
    //digitalWrite(_cs, LOW);
    
    //Serial.print("C ");
    spiwrite(c);
    
    *csport |= cspinmask;
    //digitalWrite(_cs, HIGH);
}


void FastSSD1351::writeData(uint8_t c) {
    *rsport |= rspinmask;
    //digitalWrite(_rs, HIGH);
    
    *csport &= ~ cspinmask;
    //digitalWrite(_cs, LOW);
    
//    Serial.print("D ");
    spiwrite(c);
    
    *csport |= cspinmask;
    //digitalWrite(_cs, HIGH);
} 

/***********************************/

uint16_t FastSSD1351::Color565(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c;
  c = r >> 3;
  c <<= 6;
  c |= g >> 2;
  c <<= 5;
  c |= b >> 3;

  return c;
}

void FastSSD1351::drawPixel(int16_t x, int16_t y, uint16_t color)
{
   #if (SCREEN_SENS==0)
  _buffer[(y << 7) + x ] = color;
  #endif
  
  #if (SCREEN_SENS==90)
  _buffer[((SSD1351WIDTH-1-x) << 7) + y ] = color;
  #endif

   #if (SCREEN_SENS==180)
  _buffer[((SSD1351HEIGHT-1-y) << 7) + SSD1351WIDTH - 1 - x ] = color;
  #endif

   #if (SCREEN_SENS==270)
  _buffer[ ( x << 7) + SSD1351HEIGHT-1-y ] = color;
  #endif
  
}


// printing to screen a picture from uint16_t array. RGB565 (65536 colors)

void FastSSD1351::drawBitmap16bit ( const uint16_t tab[],           // array of const uint16_t
                                    uint8_t l,                      // nb of uint16 on the width = nb of pixels
                                    uint8_t x, uint8_t y,           // screen position of the upper left corner 
                                    uint8_t x0, uint8_t y0,         // relative position in the bitmap of the box to print on screen 
                                    uint8_t w, uint8_t h,           // Width and height of the box to print
                                    uint16_t c0,                    // value of the transoarent color 
                                    int8_t rev                      // sens 1 normal -1 revert as a miror
                                )
                                
        {   
        for(uint8_t j=0; j<h; j++) {
          for(uint8_t i=0; i<w; i++) { 
                
                uint16_t p;
                if (rev > 0) p=tab[(j+y0)*l+i+x0];
                
                else p=tab[(j+y0)*l+l-i-x0];
                
                if (p!=c0) drawPixel (x+i, y+j, p);
          }   }  
          }

void FastSSD1351::drawBitmap1bit (  const uint8_t tab[],            // array of const uint8_t
                                    uint8_t l,                      // nb of uint8 in width  WARNING nb of pixels is  8 * l 
                                    uint8_t x, uint8_t y,           // position of displaying on screen 
                                    uint8_t x0, uint8_t y0,         // relative position of the box to print in the bitmap WARNING multiple of 8 pour x0
                                    uint8_t w, uint8_t h,           // lWidth and height of the box to print WARNING multiple of  8 for w
                                    uint16_t c0,                    // background color  if 0x0001 transparent color
                                    uint16_t c1 )                   // color of the 1bit image to print 
                                 {             
  
  for(uint8_t j=y0; j<y0+h; j++) {  
    for(uint8_t i=x0/8; i<(x0+w)/8; i++) {        // counting uint8  un the array  
       for(uint8_t k=0; k<8; k++) {               // counting of bits une the uint8     
        uint16_t p = (tab[l*j+i] &                // getting boolean value in the uint8   
                     (0b10000000 >> k))           // tranlating of k with mask 0b10000000                                    
                     >> (7-k);                    // and we translate to right to get the boolean 
                               
        uint8_t xp=x+8*i+k;
        uint8_t yp=y+j;                  
                
        if (p==0b1) drawPixel (xp, yp, c1);               // 0b01 =>  c1
        else if (c0!=0x0001) drawPixel (xp, yp, c0);      // 0b00  => c0 sauf si c0 == 0x0001 => transparent ... rien ne se passe
          }  }   } 
                                }

// affichage d'un chiffre en mode 7 segments

void FastSSD1351::drawDigit7seg ( 
                                  uint8_t digit,         // guess !
                                  uint8_t x, uint8_t y,  // position on screen
                                  uint16_t c0,           // couleur fond 0 (transparent si 0x0001)
                                  uint16_t c1  )        // couleur des segments 1
{           
       if (digit > 9) return;
       
       for(uint8_t k=0; k<7; k++) { // on lit de droite à gauche la valeur dans l'uint8 avec le masque 0b00000001 decale de k
                  if (segNum7  [digit] & (0b00000001 << k) ) // on relit de droite à gauche la valeur dans l'uint8 avec les masques
                  {
            uint8_t pSeg=posSeg7  [k];
              boolean px=  pSeg & 0b00000001;            // position du segments en x
              uint8_t py = (pSeg & 0b00000110) >> 1;     // position du segments en y
     
              if (pSeg & 0b00001000)        // Horizontal segment : 0 ou vertical segment : 1
                  
                                    drawBitmap1bit (     seg7V,                 
                                    1,                         
                                    x+ 23 * px, y + 5 + 23 * py,   
                                    0, 0,                     
                                    8, 20,                   
                                    c0,                   
                                    c1 );                  
              
              else                 drawBitmap1bit (     seg7H,                 
                                    3,                        
                                    x+ 3 + 23 * px, y + 1 + 23 * py,   
                                    0, 0,                              
                                    24, 5,                             
                                    c0,                                
                                    c1 );                              
          }
      }
 
}



// shade a 16 bit RGB color (565)

uint16_t FastSSD1351::colorShade(uint8_t shade, uint16_t _rgb565)
    {
     
     uint16_t red, green, blue;
                       // rrrrrggggggbbbbb
    red =   (((_rgb565 & 0b1111100000000000) >> 11) >> shade ) << 11;
    
    green = (uint16_t)(((_rgb565 & 0b0000011111100000) >> 5 ) >> shade) << 5;

    blue =  (uint16_t)(_rgb565 & 0b0000000000011111) >> shade ;

    return (uint16_t)(red | green | blue); 
    }


// shade entire screen
void FastSSD1351::screenShade ( uint8_t shade)
  {
    for(uint16_t i=0; i < SSD1351WIDTH * SSD1351HEIGHT; i++) _buffer[i] = colorShade(shade, _buffer[i]);     
      
  }

void FastSSD1351::begin(void) {
    // set pin directions
    pinMode(_rs, OUTPUT);
    
    if (_sclk) {
        pinMode(_sclk, OUTPUT);
        
        pinMode(_sid, OUTPUT);
    } else {
        // using the hardware SPI
        SPI.begin();
        SPI.setDataMode(SPI_MODE3);
    }
	
    // Toggle RST low to reset; CS low so it'll listen to us
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, LOW);
    
    if (_rst) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, HIGH);
        delay(500);
        digitalWrite(_rst, LOW);
        delay(500);
        digitalWrite(_rst, HIGH);
        delay(500);
    }

    // Initialization Sequence
    writeCommand(SSD1351_CMD_COMMANDLOCK);  // set command lock
    writeData(0x12);  
    writeCommand(SSD1351_CMD_COMMANDLOCK);  // set command lock
    writeData(0xB1);

    writeCommand(SSD1351_CMD_DISPLAYOFF);  		// 0xAE

    writeCommand(SSD1351_CMD_CLOCKDIV);  		// 0xB3
    writeCommand(0xF1);  						// 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    
    writeCommand(SSD1351_CMD_MUXRATIO);
    writeData(127);
    
    writeCommand(SSD1351_CMD_SETREMAP);
    writeData(0x74);
  
    writeCommand(SSD1351_CMD_SETCOLUMN);
    writeData(0x00);
    writeData(0x7F);
    writeCommand(SSD1351_CMD_SETROW);
    writeData(0x00);
    writeData(0x7F);

    writeCommand(SSD1351_CMD_STARTLINE); 		// 0xA1
    if (SSD1351HEIGHT == 96) {
      writeData(96);
    } else {
      writeData(0);
    }

    writeCommand(SSD1351_CMD_DISPLAYOFFSET); 	// 0xA2
    writeData(0x0);

    writeCommand(SSD1351_CMD_SETGPIO);
    writeData(0x00);
    
    writeCommand(SSD1351_CMD_FUNCTIONSELECT);
    writeData(0x01); // internal (diode drop)
    //writeData(0x01); // external bias

//    writeCommand(SSSD1351_CMD_SETPHASELENGTH);
//    writeData(0x32);

    writeCommand(SSD1351_CMD_PRECHARGE);  		// 0xB1
    writeCommand(0x32);
 
    writeCommand(SSD1351_CMD_VCOMH);  			// 0xBE
    writeCommand(0x05);

    writeCommand(SSD1351_CMD_NORMALDISPLAY);  	// 0xA6

    writeCommand(SSD1351_CMD_CONTRASTABC);
    writeData(0xC8);
    writeData(0x80);
    writeData(0xC8);

    writeCommand(SSD1351_CMD_CONTRASTMASTER);
    writeData(0x0F);

    writeCommand(SSD1351_CMD_SETVSL );
    writeData(0xA0);
    writeData(0xB5);
    writeData(0x55);
    
    writeCommand(SSD1351_CMD_PRECHARGE2);
    writeData(0x01);
    
    writeCommand(SSD1351_CMD_DISPLAYON);		//--turn on oled panel    
}


void FastSSD1351::update() 
        { 
         // Set up raw pixel dump to entire screen.  Although such writes can wrap
         // around automatically from end of rect back to beginning, the region is
         // reset on each frame here in case of an SPI glitch.
         
          SPI.beginTransaction(SPISettings(24000000, MSBFIRST, SPI_MODE3));
          
          writeCommand(SSD1351_CMD_SETROW);    // Y range
          writeData(0); writeData(SSD1351HEIGHT - 1);
          writeCommand(SSD1351_CMD_SETCOLUMN); // X range
          writeData(0); writeData(SSD1351WIDTH  - 1);
          
          writeCommand(SSD1351_CMD_WRITERAM);  // Begin write
          
          digitalWrite(_cs, LOW);  
          digitalWrite(_rs, HIGH);                         // Data mode
          
         // Now just issue raw 16-bit values for every pixel...
          for(uint16_t i=0; i<SSD1351WIDTH * SSD1351HEIGHT; i++) 
              {
             
             // SPI FIFO technique from Paul Stoffregen's ILI9341_t3 library:
                
                while(KINETISK_SPI0.SR & 0xC000); // Wait for space in FIFO
                   KINETISK_SPI0.PUSHR = _buffer[i] | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT; }   
                
                KINETISK_SPI0.SR |= SPI_SR_TCF;         // Clear transfer flag
                
                while((KINETISK_SPI0.SR & 0xF000) ||    // Wait for SPI FIFO to drain
                     !(KINETISK_SPI0.SR & SPI_SR_TCF)); // Wait for last bit out
                
                digitalWrite(_cs, HIGH);          // Deselect 
                SPI.endTransaction();
              }

void FastSSD1351::setToSleep()
  {
    writeCommand(SSD1351_CMD_DISPLAYOFF);
  }


/********************************* low level pin initialization */

FastSSD1351::FastSSD1351(uint8_t cs, uint8_t rs, uint8_t sid, uint8_t sclk, uint8_t rst) : Adafruit_GFX(SSD1351WIDTH, SSD1351HEIGHT) {
    _cs = cs;
    _rs = rs;
    _sid = sid;
    _sclk = sclk;
    _rst = rst;
    
    csport      = portOutputRegister(digitalPinToPort(cs));
    cspinmask   = digitalPinToBitMask(cs);

    rsport      = portOutputRegister(digitalPinToPort(rs));
    rspinmask   = digitalPinToBitMask(rs);

    sidport      = portOutputRegister(digitalPinToPort(sid));
    sidpinmask   = digitalPinToBitMask(sid);

    sclkport      = portOutputRegister(digitalPinToPort(sclk));
    sclkpinmask   = digitalPinToBitMask(sclk);

}

FastSSD1351::FastSSD1351(uint8_t cs, uint8_t rs,  uint8_t rst) : Adafruit_GFX(SSD1351WIDTH, SSD1351HEIGHT)
{
    _cs = cs;
    _rs = rs;
    _sid = 0;
    _sclk = 0;
    _rst = rst;

    csport      = portOutputRegister(digitalPinToPort(cs));
    cspinmask   = digitalPinToBitMask(cs);
    
    rsport      = portOutputRegister(digitalPinToPort(rs));
    rspinmask   = digitalPinToBitMask(rs);

}
