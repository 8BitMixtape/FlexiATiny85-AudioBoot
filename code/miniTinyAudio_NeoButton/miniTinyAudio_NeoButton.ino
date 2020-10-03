#include <Adafruit_NeoPixel.h>
#include "pitches.h"

// 
// 
// 
// 
// 
// 

// hardware description / pin connections
#define NEOPIXELPIN         0
#define SPEAKERPIN          1
#define BUTTONS_ADC        A3

#define NUMPIXELS           16

#define BUTTON_NONE         0
#define BUTTON_LEFT         1
#define BUTTON_RIGHT        2

#define hell          55 // Brightness

int pauseBetweenNotes = 80;
int noteDuration_ms = 150;

uint8_t showPixel = 0;
uint8_t gaggi;
uint8_t schnabi;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Specific functions of the 8Bit Mixtape NEO
 ___ _____ _ _      _____ _     _                  _____ _____ _____ 
| . | __  |_| |_   |     |_|_ _| |_ ___ ___ ___   |   | |   __|     |
| . | __ -| |  _|  | | | | |_'_|  _| .'| . | -_|  | | | |   __|  |  |
|___|_____|_|_|    |_|_|_|_|_,_|_| |__,|  _|___|  |_|___|_____|_____|
                                       |_|                           
========================================================================================================================
   _________    
  | 8Bit()  |   uint8_t   getButton()                       -> 1 left, 2 right  3 both 
  |  o___o  |   uint16_t  analogReadScaled(uint8_t channel) -> scale the adc of the voltage divider to a return value: 0..1023     
  |__/___\__|   
                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

#define Vbutton_releaseLevel  450
#define Vbutton_left          400
#define Vbutton_right         310
#define Vbutton_both          234
#define Vbutton_pressedLevel  Vbutton_left 

uint8_t getButton()
{
  uint8_t  button = BUTTON_NONE;
  uint16_t pinVoltage;
  
  pinVoltage = analogRead( BUTTONS_ADC );
  
  if ( pinVoltage < Vbutton_left  ) button = BUTTON_LEFT;
  if ( pinVoltage < Vbutton_right ) button = BUTTON_RIGHT;
  if ( pinVoltage < Vbutton_both  ) button = BUTTON_LEFT + BUTTON_RIGHT;

  return button;
}

/*
 uint8_t wasButtonPressed()
 
 returns the button which was pressed when the button is released.
 
 return values:
 
 BUTTON_LEFT
 BUTTON_RIGHT
 BUTTON_LEFT+BUTTON_RIGHT
 
 One of the most underestimated tasks is debouncing a button. 
 This routine works in the most cases. It could be useful to 
 add some time delay after the button is released to prevent
 bouncing. 
 
 v0.1  16.03.2017 -H-A-B-E-R-E-R-  first version 
 
*/

#define BUTTON_NOTPRESSED   0
#define BUTTON_PRESSED      1

uint8_t wasButtonPressed()
{
  static uint8_t buttonPressed    = false;
  static uint8_t buttonState      = 0;
  static uint8_t buttonValue      = BUTTON_NONE;
  static uint8_t buttonMaxValue   = 0;

  uint8_t        buttonReturnValue;  
  uint16_t       pinVoltage;
    
  pinVoltage = analogRead( BUTTONS_ADC );
  
  // hysteresis switch
  if( pinVoltage > Vbutton_releaseLevel ) buttonPressed = false;
  if( pinVoltage < Vbutton_pressedLevel ) buttonPressed = true;

  buttonReturnValue = BUTTON_NONE;
  
  switch( buttonState )
  {
    case BUTTON_NOTPRESSED:
    {
      buttonMaxValue = 0;      
      
      if( buttonPressed )
      {      
        buttonState = BUTTON_PRESSED;
      }
    };break;
    
    case BUTTON_PRESSED:
    {
      if( buttonPressed ) // find minimum volage level during button pressed period
      {
        buttonValue = BUTTON_NONE; 
        
             if ( pinVoltage < Vbutton_both  ) buttonValue = BUTTON_LEFT + BUTTON_RIGHT;
        else if ( pinVoltage < Vbutton_right ) buttonValue =               BUTTON_RIGHT;
        else if ( pinVoltage < Vbutton_left  ) buttonValue = BUTTON_LEFT               ;      
        
        if( buttonValue > buttonMaxValue ) buttonMaxValue = buttonValue;   
                                     
      }else
      {
        buttonState = BUTTON_NOTPRESSED;
  buttonReturnValue = buttonMaxValue; 
      }
      ;break;

    }
  }
  
  return buttonReturnValue; 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Specific functions of the NEO-Pixel Library
 _____ _____ _____        _         _ 
|   | |   __|     |   ___|_|_ _ ___| |
| | | |   __|  |  |  | . | |_'_| -_| |
|_|___|_____|_____|  |  _|_|_,_|___|_|
                     |_|              
========================================================================================================================
   _________    
  | NEO(x)  |   void setWhiteAllPixel(uint32_t color)                   -> Sets all the pixels to the white level
  |  o___o  |   void displayBinrayValue(uint16_t value, uint32_t color) -> displays binary number
  |__/___\__|   uint32_t Wheel(byte WheelPos)                           -> Input a value 0 to 255 to get a color value.   
                                                                        The colours are a transition r - g - b - back to r. 
                                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void setWhiteAllPixel(uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    pixels.setPixelColor(n, color, color, color);
  }
}

void setColorAllPixel(uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    pixels.setPixelColor(n, color);
  }
}

void displayBinrayValue(uint16_t value, uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    if (value & (1 << n)) pixels.setPixelColor(n, color);
    //else pixels.setPixelColor(n,0); // off
  }
}

void rainbowCycle(uint8_t wait, uint8_t rounds, uint8_t rainbowPixels) {
  uint16_t i, j;

  for (j = 0; j < 256 * rounds; j++) { 
    for (i = 0; i < rainbowPixels; i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / rainbowPixels) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// fast pin access
#define AUDIOPIN (1<<SPEAKERPIN)
#define PINLOW (PORTB&=~AUDIOPIN)
#define PINHIGH (PORTB|=AUDIOPIN)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Play simple sounds and tunes
========================================================================================================================
   _________          
  |playTune |  
  |  o___o  |  
  |__/___\__| 
   
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

int FamilyMart[] = {
  NOTE_B4, NOTE_G4, NOTE_D4, NOTE_G4, NOTE_A4, NOTE_D5, PAUSE, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_D4, NOTE_G4 
};

void playSound(long freq_Hz, long duration_ms)
{
  uint16_t n;
  uint32_t delayTime_us;
  uint32_t counts;

  delayTime_us = 1000000UL / freq_Hz / 2;
  counts = duration_ms * 1000 / delayTime_us;

  for (n = 0; n < counts; n++)
  {
    PINHIGH;
    delayMicroseconds(delayTime_us);
    PINLOW;
    delayMicroseconds(delayTime_us);
  }
}

void playTune(int notes[])
{
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote <= 11; thisNote++) {

    displayBinrayValue(notes[thisNote],pixels.Color(50,0,50));
    pixels.show(); // This sends the updated pixel color to the hardware.
    playSound( notes[thisNote], noteDuration_ms);
    setColorAllPixel(0); // pixels off
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    delay(pauseBetweenNotes);
        
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* the setup routine runs once when you start the tape or press reset
========================================================================================================================
   _________            _                   
  | setup() |   ___ ___| |_ _ _ _ _ 
  |  o___o  |  |_ -| -_|  _| | | . |
  |__/___\__|  |___|___|_| |___|  _|
                               |_|    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


void setup(void) {

  pinMode(SPEAKERPIN, OUTPUT);
  uint8_t brightness = hell;
  pixels.begin();
  pixels.setBrightness(brightness);
  
  rainbowCycle(2,1,16);
  
  delay(10);
  
  setColorAllPixel(Wheel(220));
  pixels.setBrightness(brightness);
  pixels.show(); // Initialize all pixels to 'off'
    
  //playTune(FamilyMart);
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The main loop to put all your code
========================================================================================================================
   _________    _    
  | loop()  |  | |___ ___ ___ 
  |  o___o  |  | | . | . | . | 
  |__/___\__|  |_|___|___|  _| 
                         |_| 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void loop(void) { 
  
  uint8_t b = wasButtonPressed();
  
  //uint8_t b = getButton();
  if( b == BUTTON_LEFT                )   
    {
      gaggi=gaggi+50;
      pixels.setBrightness(gaggi);
      //pixels.show(); // Initialize all pixels to 'off'
    }
  if( b == BUTTON_RIGHT               )   
    {
      //rainbowCycle(2,1,16);
      schnabi=schnabi+50;
      setColorAllPixel(Wheel(schnabi));
      //pixels.show(); // Initialize all pixels to 'off'
    }  
  
  if( b == BUTTON_RIGHT + BUTTON_LEFT )   
    {
      rainbowCycle(10,10,100);
    }
  pixels.show(); // Initialize all pixels to 'off'

}
