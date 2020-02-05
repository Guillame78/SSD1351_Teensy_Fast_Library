 
#include <Adafruit_GFX.h>

#define SSD1351WIDTH 128
#define SSD1351HEIGHT 128  // SET THIS TO 96 FOR 1.27"!

#define SCREEN_SENS 90  // SET SCREEN ORIENTATION in degrees TRIGO SENS

#define swap(a, b) { uint16_t t = a; a = b; b = t; }

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// #ifdef __SAM3X8E__
#if defined (__SAM3X8E__) || (ARDUINO_ARCH_SAMD) 
    typedef volatile RwReg PortReg;
    typedef uint32_t PortMask;
#elif defined (ESP8266)
    typedef volatile uint32_t PortReg;
    typedef uint32_t PortMask;
#else
    typedef volatile uint8_t PortReg;
    typedef uint8_t PortMask;
#endif

// Select one of these defines to set the pixel color order
#define SSD1351_COLORORDER_RGB
// #define SSD1351_COLORORDER_BGR

#if defined SSD1351_COLORORDER_RGB && defined SSD1351_COLORORDER_BGR
  #error "RGB and BGR can not both be defined for SSD1351_COLORODER."
#endif

// Timing Delays
#define SSD1351_DELAYS_HWFILL	    (3)
#define SSD1351_DELAYS_HWLINE       (1)

// SSD1351 Commands
#define SSD1351_CMD_SETCOLUMN 		0x15
#define SSD1351_CMD_SETROW    		0x75
#define SSD1351_CMD_WRITERAM   		0x5C
#define SSD1351_CMD_READRAM   		0x5D
#define SSD1351_CMD_SETREMAP 		0xA0
#define SSD1351_CMD_STARTLINE 		0xA1
#define SSD1351_CMD_DISPLAYOFFSET 	0xA2
#define SSD1351_CMD_DISPLAYALLOFF 	0xA4
#define SSD1351_CMD_DISPLAYALLON  	0xA5
#define SSD1351_CMD_NORMALDISPLAY 	0xA6
#define SSD1351_CMD_INVERTDISPLAY 	0xA7
#define SSD1351_CMD_FUNCTIONSELECT 	0xAB
#define SSD1351_CMD_DISPLAYOFF 		0xAE
#define SSD1351_CMD_DISPLAYON     	0xAF
#define SSD1351_CMD_PRECHARGE 		0xB1
#define SSD1351_CMD_DISPLAYENHANCE	0xB2
#define SSD1351_CMD_CLOCKDIV 		0xB3
#define SSD1351_CMD_SETVSL 		0xB4
#define SSD1351_CMD_SETGPIO 		0xB5
#define SSD1351_CMD_PRECHARGE2 		0xB6
#define SSD1351_CMD_SETGRAY 		0xB8
#define SSD1351_CMD_USELUT 		0xB9
#define SSD1351_CMD_PRECHARGELEVEL 	0xBB
#define SSD1351_CMD_VCOMH 		0xBE
#define SSD1351_CMD_CONTRASTABC		0xC1
#define SSD1351_CMD_CONTRASTMASTER	0xC7
#define SSD1351_CMD_MUXRATIO            0xCA
#define SSD1351_CMD_COMMANDLOCK         0xFD
#define SSD1351_CMD_HORIZSCROLL         0x96
#define SSD1351_CMD_STOPSCROLL          0x9E
#define SSD1351_CMD_STARTSCROLL         0x9F


class FastSSD1351  : public virtual Adafruit_GFX {
 public:
  FastSSD1351(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST);
  FastSSD1351(uint8_t CS, uint8_t RS, uint8_t RST);

  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

  // drawing primitives!
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  // drawing bitmaps
    
    void drawBitmap16bit (const uint16_t tab[],          //tableau
                          uint8_t l,                // nb de uint16 en largeur = nb de pixels
                          uint8_t x, uint8_t y,   // position d'affichage sur l'écran
                          uint8_t x0, uint8_t y0,   // position relative dans le bitmap du rectangle à afficher
                          uint8_t w, uint8_t h,      // largeur et hauteur du rectangle à afficher
                          uint16_t c0,             // couleur transparente 
                          int8_t rev               // sens
                          );
    
    void drawBitmap1bit ( const uint8_t tab[],      //tableau
                          uint8_t l,                // nb de uint8 en largeur du tableau WARNING nb de pixels en largeur = 8 * l 
                          uint8_t x, uint8_t y,     // position d'affichage sur l'écran
                          uint8_t x0, uint8_t y0,   // position relative dans le bitmap du rectangle à afficher
                          uint8_t w, uint8_t h,     // largeur et hauteur du rectangle à afficher
                          uint16_t c0,           // couleur pour 0 (transparent si 0x0001)
                          uint16_t c1  );        // couleur pour 1
    
    void drawDigit7seg (  uint8_t digit,
                          uint8_t x, uint8_t y,     // position d'affichage sur l'écran
                          uint16_t c0,           // couleur fond 0 (transparent si 0x0001)
                          uint16_t c1  );        // couleur des segments 1
  
  // Shading screen

  uint16_t colorShade(uint8_t shade, uint16_t _rgb565);

  void screenShade (uint8_t shade);

  // commands
  void begin(void);

  void reset(void);

  void update();

  void setToSleep();

  /* low level */

  void writeData(uint8_t d);
  void writeCommand(uint8_t c);


  void writeData_unsafe(uint16_t d);

  void setWriteDir(void);
  void write8(uint8_t d);

 private:
  
  void spiwrite(uint8_t);
  
  uint8_t _cs, _rs, _rst, _sid, _sclk;
  PortReg *csport, *rsport, *sidport, *sclkport;
  PortMask cspinmask, rspinmask, sidpinmask, sclkpinmask;

  uint16_t _buffer[ SSD1351WIDTH * SSD1351HEIGHT ];
};
