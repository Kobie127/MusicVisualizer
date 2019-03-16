#include <FastLED.h>
#include <LiquidCrystal.h> //Includes the LCD library
#include <Arduino.h>


/**
 * This code is for the Led music visualizer that will be put 
 * into the smart car that is being developed by the Computer
 * Sceince House @RIT.  This arduino setup will visualize the
 * music by the beat.  It will also have the feature to adjust
 * sensitivity and brightness.  This sketch will also include
 * different "modes" to see which one fits best.
 * @author Kobie Arndt
 * @version 3/15/19
 */

 //The following code uses the SparkFun Spectrum Shield for audio input

 //This is the LED lighting setup.

// LED LIGHTING SETUP
#define LED_PIN     6
#define NUM_LEDS    100 // 250
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define UPDATES_PER_SECOND 100

//Audio input setup
int strobe = 4;
int reset = 5;
int audio1 = A0;
int audio2 = A1;
int left[7];
int right[7];
int band;
int audio_input = 0;
int freq = 0;

//Visualizer variables used for the led's
int midway = NUM_LEDS / 2; //The center mark form the double level visualizer.
int loop_max = 0;
int k = 255; //Color wheel position otherwise known as the RGB scale.
int decay = 0; //This will keep track of the milliseconds before a light decays.
int decay_check = 0;
long pre_react = 0; //This is the new spike conversion
long react = 0; //The number of led's being lit at once.
long post_react = 0; //This is the old spike conversion

int wheel_speed = 2;

/**
 * This is the void setup.  What the setup is for is to initialize the 
 * functionality of the spectrum shield and the lcd screen to control the
 * brightness and sensitivity of the lights.
 */
void setup(){

 //Setup for spectrum shield
 pinMode(audio1, INPUT); //This line of code takes in audio1 as a input
 pinMode(audio2, INPUT): //This line of code takes in audio2 as a input
 pinMode(strobe, OUTPUT); //This takes the strobe int and outputs it to the light
 pinMode(reset, OUTPUT);
 digitalWrite(reset, LOW);
 digitalWrite(strobe, HIGH);

 //Lighting setup
 
 delay( 3000 );
 FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

//Clears LEDS
 for(int i = 0; i < NUM_LEDS; i++)
  leds[i] = CRGB(0, 0, 0);
 FastLED.show();

  //Serial and input setup
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.clear();
  Serial.println("\nListening...");
 }

/**
 * This function simulates the scroll.  What this does is
 * takes in the int pos and generates a color based 
 * on virtual wheel.
 */
 CRGB Scroll(int pos){
  CRGB color (0,0,0);
  if(pos < 85){
    color.g = 0;
    color.r = ((float)pos / 85.0f) * 255.0f;
    color.b = 255 - color.r;
  }else if (pos < 170){
    color.g = ((float)(pos - 85) / 85.0f) * 255.0f;
  }
 }


 
