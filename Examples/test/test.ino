
#include <FastSSD1351.h>
#include "Config.h"
#include "Data.h"

FastSSD1351 oled = FastSSD1351(DISPLAY_CS_PIN, DISPLAY_DC, 0);

elapsedMillis oledRefresh;
elapsedMillis oledUpdate;

void setup() 
{
oled.begin(); 
}

void loop() 

{
static uint16_t s;

static uint32_t oledCount;
s++;

for(uint8_t j=0; j<S_HEIGHT; j++)                         // y pixel counting over the 128 pixel of the screen
      {
        for(uint8_t i=0; i<S_WIDTH; i++)                  // x pixel counting over the 128 pixel of the screen 
                 {
                  uint16_t k = (i+s) & (S_WIDTH-1) ;
                  oled.drawPixel (i, j, backGround2[(j * S_WIDTH) + k]);
                 }
      }

oled.setTextColor(WHITE);
oled.setCursor(0, 0); 
oled.print("hello World"); 
oled.setCursor(0, 8); 
oled.print("Refesh time : "); oled.print(oledCount); 

uint8_t thousand, hundred, ten, unit;

uint16_t n = millis() / 1000;

thousand = n / 1000;
hundred  = (n - 1000 * thousand) / 100;
ten = (n - 1000 * thousand - 100 * hundred) / 10;
unit = n - 1000 * thousand - 100 * hundred - 10 * ten;

oled.drawDigit7seg (      thousand,  5,  40, 0x0001, BLACK  );  
oled.drawDigit7seg (      hundred,   35, 40, 0x0001, BLACK  );  
oled.drawDigit7seg (      ten,       65, 40, 0x0001, BLACK  );  
oled.drawDigit7seg (      unit,      95, 40, 0x0001, BLACK  );  
                                
if (oledRefresh > OLED_REFRESH) 
  {
    oledUpdate = 0;
    oledRefresh = 0;
    oled.update();
    oledCount = oledUpdate;
  }

}
