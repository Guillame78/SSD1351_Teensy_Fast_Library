#ifndef TYPEDEF_H
#define TYPEDEF_H

#define S_WIDTH  128
#define S_HEIGHT 128

// TIME CONSTANTS ----------------------------------------------------------------------------
    #define OLED_REFRESH         60

// HARDWARE CONFIG ----------------------------------------------------------------------------
   
    // DISPLAY
    #define DISPLAY_DC        15 // Data command 9 by default
    #define DISPLAY_RESET     14 // Reset 
    #define DISPLAY_CS_PIN    10 // Display select 
    #define DISPLAY_CLK       13 // Clock
    #define DISPLAY_DATA      11 // Data

    // BUZZER
    #define BUZZER_PIN        5 // 
    
    // SWITCH
    #define UP_PIN            17
    #define MID_PIN           9
    #define DOWN_PIN          16

    // JOYSTICK
    #define JOYX_PIN          A6
    #define JOYY_PIN          A7

// RGB 565 colors  ----------------------------------------------------------------------------

#define   BLACK            0x0000
#define   BLUE             0x001F
#define   RED              0xF800
#define   GREEN            0x07E0
#define   CYAN             0x07FF
#define   MAGENTA          0xF81F
#define   YELLOW           0xFFE0  
#define   WHITE            0xFFFF


#endif
