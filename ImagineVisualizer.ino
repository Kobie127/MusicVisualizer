#include <FastLED.h>
#include <LiquidCrystal.h> //Includes the LCD library
#include <Arduino.h>
#include <Wire.h>
#include <U8x8lib.h> //This library is used for setting fonts on the LCD screen.

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif


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

//Screen for button setup variables
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display (you may have to edit the address of your display)
int upB = 12;
int downB = 11;
int selectB = 3;
int menu = 1;


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
 FastLED.setBrightness(BIRGHTNESS);

//Clears LEDS
 for(int i = 0; i < NUM_LEDS; i++)
  leds[i] = CRGB(0, 0, 0);
 FastLED.show();

  //Serial and input setup
  Serial.begin(115200);
  Serial.println("\nListening...");
 }

 /**
  * This function is to setup the lcd display screen which
  * will hold all of the functionality to switch in between
  * measuring the different kinds of frequencies and visualize
  * that specific frequency that the user desires.
  */
 void setupDisplay(){
    u8x8.begin();
    u8x8.setFont(u8x8_font_chroma48medium8_r);  
    pinMode(upB, INPUT_PULLUP);
    pinMode(downN, INPUT_PULLUP); 
    pinMode(selectB, INPUT_PULLUP);
    updateMenu();  
 }

 void setupFreqDisplay(){
  u8x8.clear();
  u8x8.drawString(0,0, "-Listening....");

  switch(freq){
    case 0:
      u8x8.drawString(0,2, "Freq is 63 Hz");
      u8x8.drawString(0,7, "Made by Kobie Arndt");
      break;
    case 1:
      u8x8.drawString(0,2, "Freq is 160 Hz");
      u8x8.drawString(0,7, "Made by Kobie Arndt");
      break;
    case 2: 
      u8x8.drawString(0,2, "Freq is 400 Hz");
      u8x8.drawString(0,7, "Made by Kobie Arndt");
      break;
    case 3:
      u8x8.drawString(0,2, "Freq is 1 kHz");
      u8x8.drawString(0,7, "Made by Kobie Arndt");
      break;
    case 4:
      u8x8.drawString(0,2, "Freq is 2.5 kHz");
      u8x8.drawString(0,7, "Made by Kobie Arndt");
      break;
    case 5:
      u8x8.drawString(0,2, "Freq is 6.25 kHz");
      u8x8.drawString(0,7, "Made by Kobie Arndt");
      break;
    case 6:
      u8x8.drawString(0,2, "Freq is 16 kHz");
      u8x8.drawString(0,7, "Made by Kobie Arndt");
      break;
  }
 }


 void updateMenu(){
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CRGB(0, 0, 0);
  FastLED.show();

  switch(menu){
    case 0:
      menu = 1;
      break;
    case 1:
      u8x8.drawString(0,0, "-----------------");
      u8x8.drawString(0,1, ">63      Hz");
      u8x8.drawString(0,2," 160     Hz");
      u8x8.drawString(0,3," 400     Hz");
      u8x8.drawString(0,4, "1      kHz");
      u8x8.drawString(0,5, "2.5    kHz");
      u8x8.drawString(0,6, "6.25   kHz");
      u8x8.drawString(0,7, "16     kHz");
      u8x8.drawString(0,8,"-----------------");
      break;
    case 2:
      u8x8.drawString(0,0, "-----------------");
      u8x8.drawString(0,1, "63      Hz");
      u8x8.drawString(0,2," >160     Hz");
      u8x8.drawString(0,3," 400     Hz");
      u8x8.drawString(0,4, "1      kHz");
      u8x8.drawString(0,5, "2.5    kHz");
      u8x8.drawString(0,6, "6.25   kHz");
      u8x8.drawString(0,7, "16     kHz");
      u8x8.drawString(0,8,"-----------------");
      break;
    case 3:
      u8x8.drawString(0,0, "-----------------");
      u8x8.drawString(0,1, "63      Hz");
      u8x8.drawString(0,2," 160     Hz");
      u8x8.drawString(0,3," >400     Hz");
      u8x8.drawString(0,4, "1      kHz");
      u8x8.drawString(0,5, "2.5    kHz");
      u8x8.drawString(0,6, "6.25   kHz");
      u8x8.drawString(0,7, "16     kHz");
      u8x8.drawString(0,8,"-----------------");
      break;
    case 4:
      u8x8.drawString(0,0, "-----------------");
      u8x8.drawString(0,1, "63      Hz");
      u8x8.drawString(0,2," 160     Hz");
      u8x8.drawString(0,3," 400     Hz");
      u8x8.drawString(0,4, ">1     kHz");
      u8x8.drawString(0,5, "2.5    kHz");
      u8x8.drawString(0,6, "6.25   kHz");
      u8x8.drawString(0,7, "16     kHz");
      u8x8.drawString(0,8,"-----------------");
      break;
    case 5:
      u8x8.drawString(0,0, "-----------------");
      u8x8.drawString(0,1, "63      Hz");
      u8x8.drawString(0,2, "160     Hz");
      u8x8.drawString(0,3, "400     Hz");
      u8x8.drawString(0,4, "1      kHz");
      u8x8.drawString(0,5, ">2.5   kHz");
      u8x8.drawString(0,6, "6.25   kHz");
      u8x8.drawString(0,7, "16     kHz");
      u8x8.drawString(0,0, "-----------------");
      break;
    case 6:
      u8x8.drawString(0,0, "-----------------");
      u8x8.drawString(0,1, "63      Hz");
      u8x8.drawString(0,2, "160     Hz");
      u8x8.drawString(0,3, "400     Hz");
      u8x8.drawString(0,4, "1      kHz");
      u8x8.drawString(0,5, "2.5   kHz");
      u8x8.drawString(0,6, ">6.25   kHz");
      u8x8.drawString(0,7, "16     kHz");
      u8x8.drawString(0,0, "-----------------");
      break;
    case 7:
      u8x8.drawString(0,0, "-----------------");
      u8x8.drawString(0,1, "63      Hz");
      u8x8.drawString(0,2, "160     Hz");
      u8x8.drawString(0,3, "400     Hz");
      u8x8.drawString(0,4, "1      kHz");
      u8x8.drawString(0,5, "2.5   kHz");
      u8x8.drawString(0,6, "6.25   kHz");
      u8x8.drawString(0,7, ">16     kHz");
      u8x8.drawString(0,0, "-----------------");
      break;
    case 8:
      menu = 7;
      break;
  }
 }

 void select1(){
  u8x8.clear();
  u8x8.drawString(0,0, "-Frequency selected");
  u8x8.drawString(0,1, "63 Hz");
  freq = 0;
  break;
 }

 void select2(){
  u8x8.clear();
  u8x8.drawString(0,0, "-Frequency selected");
  u8x8.drawString(0,1, "160 Hz");
  freq = 1;
  break;
 }

 void select3(){
  u8x8.clear();
  u8x8.drawString(0,0, "-Frequency selected");
  u8x8.drawString(0,1, "400 Hz");
  freq = 2;
  break;
 }

 void select4(){
  u8x8.clear();
  u8x8.drawString(0,0, "-Frequency selected");
  u8x8.drawString(0,1, "1 kHz");
  freq = 3;
  break;
 }

 void select5(){
  u8x8.clear();
  u8x8.drawString(0,0, "-Frequency selected");
  u8x8.drawString(0,1, "2.5 kHz");
  freq = 4;
  break;
 }

 void select6(){
  u8x8.clear();
  u8x8.drawString(0,0, "-Frequency selected");
  u8x8.drawString(0,1, "6.25 kHz");
  freq = 5;
  break;
 }

 void select7(){
  u8x8.clear();
  u8x8.drawString(0,0, "-Frequency selected");
  u8x8.drawString(0,1, "16 kHz");
  freq = 6;
  break;
 }

 void selectAction(){
  switch(menu){
    case 1:
      select1();
      break;
    case 2:
      select2();
      break;
    case 3:
      select3();
      break;
    case 4:
      select4();
      break;
    case 5:
      select5();
      break;
    case 6:
      select6();
      break;
    case 7:
      select7();
      break;
  }
 }
 

  
 

/**
 * This function simulates the scroll.  What this does is
 * takes in the int pos and generates a color based
 * on virtual wheel.
 */
 CRGB Scroll(int pos){
  CRGB color (0,0,0);
  if(pos < 85){ //This checks to see if the rgb scale is below 85
    color.g = 0; //This will set it to 0
    color.r = ((float)pos / 85.0f) * 255.0f;
    color.b = 255 - color.r;
  }else if (pos < 170){
    color.g = ((float)(pos - 85) / 85.0f) * 255.0f;
    color.r = 255 - color.g;
    color.b = 0;
  }else if(pos < 256){
    color.b = ((float)(pos - 170) / 85.0f) * 255.0f;
    color.g = 255 - color.b;
    color.r = 1;
  }
  return color;
 }

/**
 * This function is meant to get and set the color of the LED.
 * This function has been modified to send the waves of colors out
 * from the first LED.
 */
 void singleR(){
  for(int i = NUM_LEDS -  1; i >= midway; i--){
    if(i > react){
        leds[i] = CRGB(0, 0 ,0);
    }else{
        leds[i] = Scroll((i * 256 / 50 + k) % 256);
  }
  FastLED.show();
 }
 }

/**
 * This function is a mirrored visualization.
 */
 void doubleR(){
  for(int i = NUM_LEDS - 1; i >= midway; i--){
    if(i > react + midway){
      leds[i[ = CRGB(0 ,0 ,0);
       leds[midway - react] = CRGB(0 ,0 ,0);
    }else{
      leds[i] = Scroll((i * 256 / 50 + k) % 256);
      leds[(midway - i) + midway] = Scroll((i * 256 / 50 + k) % 256);
    }
  }
  FastLED.show();
 }

/**
 * This function reads 7 band equalizers through the
 * SprakFun Spectrum Shield.  Reading the 7 bands
 * will return the strengths of the audio at seven
 * points.  The seven points it reads it at is -63Hz,
 * 160Hz, 400Hz, 1kHz, 2.5 kHz, 6.25 kHz then 16kHz
 * then back to 63 Hz.
 * This code was referenced from
 * https://tronixstuff.com/2013/01/31/tutorial-arduino-and-the-msgeq7-spectrum-analyzer/
 */
 void readMSGEQ7(){
  digitalWrite(res, HIGH);
  digitalWrite(res, LOW);
  for(band = 0; band < 7; band++){
    digitalWrite(strobe, LOW);//Strobe pin on the shield - kicks the IC up to the next band
    delayMicroseconds(30);
    left[band] = analogRead(0);//Store left band reading
    right[band] = analogRead(1);//And it to the right
    digitalWrite(strobe, HIGH);
  }
 }

/**
 * This function will take the lights and convert it
 * to a signle frequency.  This is one of the modes
 * that could possibly be used.
 */
 void convertSingle()
 {
   if (left[freq] > right[freq])
     audio_input = left[freq];
   else
     audio_input = right[freq];

   if (audio_input > 80)
   {
     pre_react = ((long)NUM_LEDS * (long)audio_input) / 1023L; 

     if (pre_react > react) 
       react = pre_react;

     Serial.print(audio_input);
     Serial.print(" -> ");
     Serial.println(pre_react);
   }
 }

/**
 * This function will take the lights and convert it
 * to a double frequency.  This is one of the modes
 * that could possibly be used.
 */
 void convertDouble()
 {
   if(left[freq] > right[freq])
     audio_input = left[freq];
   else
     audio_input = right[freq];

   if(audio_input > 80)
   {
     pre_react = ((long)midway * (long)audio_input) / 1023L;

     if(pre_react > react) 
       react = pre_react;

     Serial.print(audio_input);
     Serial.print(" -> ");
     Serial.println(pre_react);
   }
 }

/**
 * This function will read the 7 band equalizer using the 
 * readMSGEQ7 function and will then use the function
 * to convert it to single it then calls the singleR
 * function which will apply the color.  It also 
 * holds the functionality for keeping track of 
 * the speed of the color wheel and it can also 
 * reset the color wheel.  It also handles 
 * the functionality of removing the led's.
 */
 void singleLevel(){
   readMSGEQ7(); //This will read the 7 bands
   convertSingle();//This will convert it to a single level
   singleR(); //This will apply the color

   k = k - wheel_speed;//This is the speed of the color wheel

   if(k < 0){ //This will reset the color wheel
     k = 255;
   }

   //This will remove the led's.
   decay_check++;
   if(decay_check > decay){
     decay_check = 0;
     if(react > 0){
       react--;
     }
   }
 }

/**
 * This function will read the 7 band equalizer using the 
 * readMSGEQ7 function and will then use the function
 * to convert it to double it then calls the doubleR
 * function which will apply the color.  It also 
 * holds the functionality for keeping track of 
 * the speed of the color wheel and it can also 
 * reset the color wheel.  It also handles 
 * the functionality of removing the led's.
 */
 void doubleLevel(){
   readMSGEQ7();
   convertDouble();
   doubleR();

   k = k - wheel_speed;//This is the speed of the color wheel

   if(k < 0){ //This will reset the color wheel
     k = 255;
   }

   //This will remove  the led's.
   decay_check++;
   if(decay_check > decay){
     decay_check = 0;
     if(react > 0){
       react--;
     }
   }
   
 }

 void loop(){
   //
 }
   
