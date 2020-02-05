# SSD1351_Teensy_Fast_Library
 This library is issued from adafruit SSD1351 library and  https://github.com/adafruit/Uncanny_Eyes)
 
 All printing function to screen are directed in a uint16_t (RGB565) buffer array and the whole frame is dumped at the end of the loop with a very fast routine. The routine is issued from adafruit uncanny eyes project https://github.com/adafruit/Uncanny_Eyes)
 
 API is the most simple as you can 
  ::update to dump the buffer in one time to screen 
 I added some useful functions to print bitmap image on the screen 
  ::drawBitmap1bit   : for 1 bit images
  ::drawBitmap16bit : for 16 bit images
 
 and a 7 segments digit display function for clock application
  ::drawDigit7seg

 Simple sleeping method to close the screen
  ::setGoToSleep

---------------------------------------------------------------
  This is a library for the 1.5" & 1.27" 16-bit Color OLEDs 
  with SSD1331 driver chip

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1431
  ------> http://www.adafruit.com/products/1673

  These displays use SPI to communicate, 4 or 5 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
