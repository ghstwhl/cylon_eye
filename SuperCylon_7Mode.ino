/*****************************************************************************
 * SuperCylon - Seven Mode
 *
 * Admit it, the first thing you though about when you saw a string of 
 * Total Control Lighting Pixels was making a Cylon Eye for the front of
 * of your car.  If you are willing to admit it, and take the first step,
 * this sketch is for you.  :)
 *
 * This code is written for the following parts list:
 *  - Total Control Lighting Developer's Shield + Enclosure
 *  - Total Control Pixed Strand (25 pixels)
 *  - 9 pixel segment of high-density WS8212 pixels (NEOPixel or Total Control) 
 *    The data pin on this strip is connected to D8 on the TCL Dev Sheild
 * 
 * This code requires the following libraries:
 *   - Arduino Total Control Lighting Library
 *     https://bitbucket.org/devries/arduino-tcl
 *   - Adafruit NeoPixel library
 *     https://github.com/adafruit/Adafruit_NeoPixel
 * 
 * 
 * This sketch will allow you to use the potentiometers on the Total Control
 * Lighting Developer Shield to adjust the colors of the LEDs and select an
 * appropriate color. Push the first button to send the command which
 * generates this color back over the serial port. The colors will cascade
 * the lighting strand as you adjust the potentiometers. Be sure to open the
 * serial monitor to see the color information on your computer.
 * 


 * Mode selection:
 * 
 *  Switched modes:
 *
 *   Cylon eye: *Speed & Color Selectable
 *    * -  (d6 switch on)
 *    * -  (d7 switch on)
 *
 *   Constant band of color: *Color Selectable
 *    - *  (d6 switch off)
 *    - *  (d7 switch off)
 *
 *   Hippie Catcher:
 *    * -  (d6 switch on)
 *    - *  (d7 switch off)
 *
 *   Pulsing band of color: *Speed & Color Selectable
 *    - *  (d7 switch off)
 *    * -  (d6 switch on)
 * 
 * 
 *  Momentary modes: 
 * 
 *   Flashing White:
 *    *  (Press top button only)
 *    -
 * 
 *   Flashing Yellow:
 *    -
 *    *  (Press bottom button only)
 * 
 *   Mode Number Seven - DO NOT ACTIVATE, UNDER PENALTY OF LAW
 *    Seriously, if you push those two buttons at the same time, it's on you.
 *    I'm so serious, that I'm not even going to tell you which two buttons to press
 *    to activate Mode Number Seven.
 * 
 * 
 * Dynamic Adjustments:
 * 
 *  Speed:    Blue:
 *   * -       - *
 *   - -       - -
 * 
 *  Red:      Green:
 *   - -       - -
 *   * -       - *
 * 
 * 
 * 
 * Copyright 2014 Chris O'Halloran - cknight __ ghostwheel _ com
 * Copyright Chris O'Halloran
 * License: Attribution Non-commercial Share Alike (by-nc-sa) (CC BY-NC-SA 4.0)
 * https://creativecommons.org/licenses/by-nc-sa/4.0/
 * https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode
****************************************************************************/
#include <SPI.h>
#include <TCL.h>
#include <Adafruit_NeoPixel.h>

// This sets the number of LEDs in the TCL strand:
#define LEDCOUNT 25
const int TOPLED = LEDCOUNT -1; // Do the math once, and be done with it

// Define the min and max delay between iterations
#define DELAYLOW 10
#define DELAYHIGH 150

#define LEOBLINK 6
#define LEODELAY 75

boolean BLACKTAIL = true; // This value is overwritten by switch #2 (pin 7)
int TAILMIN = 0;
int TAILMAX = 10;

// These are p
int DIRECTION = 1; // This sets the 'direction' of movement.  

int i = 0; // A global variable for looping

int SWITCHSTATE; // A single point of reference for the state of the switches

#define PREVIEWDIM 4
#define PREVIEWPIXELS 9


/* Current values for the pixels are stored in the following three arrays */
byte red_values[LEDCOUNT];
byte green_values[LEDCOUNT];
byte blue_values[LEDCOUNT];

Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, 8, NEO_GRB + NEO_KHZ800);


// Absolute colors for the pixels
byte RED = 0;
byte BLUE = 0;
byte GREEN = 0;
float ratioRED;
float ratioBLUE;
float ratioGREEN;
int colorSUM;
float ratioHIGHEST;

int red_modifier = 25;
int green_modifier = 25;
int blue_modifier = 25;

void setup() {

  TCL.begin(); // Begin protocol for communicating with the TCL strand
  TCL.setupDeveloperShield(); // Set up developer shield for inputs

  Serial.begin(9600); // Start serial communication at 9600 baud

  strip.begin();
  reset_strand();


  pinMode(6, INPUT);           // set pin to input
  digitalWrite(6, HIGH);       // turn on pullup resistors
  pinMode(7, INPUT);           // set pin to input
  digitalWrite(7, HIGH);       // turn on pullup resistors
  CheckSwitches();

  randomize_colors();
}

void loop() {
  CheckSwitches();

  switch (SWITCHSTATE) {
    case 3:
      cylon_eye();
      break;
    case 2:
      hippie_catcher();
      break;
    case 1:
      pulsing_light();
      break;
    case 0:
      constant_light();
      break;
    }
}


void update_strand() {
  int i;  // A local instance of 'i' so we don't interfere with other loops
  
    TCL.sendEmptyFrame();
  for(i=0;i<LEDCOUNT;i++) {
    TCL.sendColor(red_values[i],green_values[i],blue_values[i]);
  }
  TCL.sendEmptyFrame();
  
  strip.setPixelColor(0, ((red_values[0] + red_values[1] + red_values[2])/ (3 * PREVIEWDIM)), ((green_values[0] + green_values[1] + green_values[2])/ (3 * PREVIEWDIM)), ((blue_values[0] + blue_values[1] + blue_values[2])/ (3 * PREVIEWDIM)));
  strip.setPixelColor(1, ((red_values[3] + red_values[4] + red_values[5])/ (3 * PREVIEWDIM)), ((green_values[3] + green_values[4] + green_values[5])/ (3 * PREVIEWDIM)), ((blue_values[3] + blue_values[4] + blue_values[5])/ (3 * PREVIEWDIM)));
  strip.setPixelColor(2, ((red_values[6] + red_values[7] + red_values[8])/ (3 * PREVIEWDIM)), ((green_values[6] + green_values[7] + green_values[8])/ (3 * PREVIEWDIM)), ((blue_values[6] + blue_values[7] + blue_values[8])/ (3 * PREVIEWDIM)));
  strip.setPixelColor(3, ((red_values[9] + red_values[10] + red_values[11])/ (3 * PREVIEWDIM)), ((green_values[9] + green_values[10] + green_values[11])/ (3 * PREVIEWDIM)), ((blue_values[9] + blue_values[10] + blue_values[11])/ (3 * PREVIEWDIM)));
  strip.setPixelColor(4, (red_values[12]/4), (green_values[12]/4), (blue_values[12]/ PREVIEWDIM ));
  strip.setPixelColor(5, ((red_values[13] + red_values[14] + red_values[15])/ (3 * PREVIEWDIM)), ((green_values[13] + green_values[14] + green_values[15])/ (3 * PREVIEWDIM)), ((blue_values[13] + blue_values[14] + blue_values[15])/ (3 * PREVIEWDIM)));
  strip.setPixelColor(6, ((red_values[16] + red_values[17] + red_values[18])/ (3 * PREVIEWDIM)), ((green_values[16] + green_values[17] + green_values[18])/ (3 * PREVIEWDIM)), ((blue_values[16] + blue_values[17] + blue_values[18])/ (3 * PREVIEWDIM)));
  strip.setPixelColor(7, ((red_values[19] + red_values[20] + red_values[21])/ (3 * PREVIEWDIM)), ((green_values[19] + green_values[20] + green_values[21])/ (3 * PREVIEWDIM)), ((blue_values[19] + blue_values[20] + blue_values[21])/ (3 * PREVIEWDIM)));
  strip.setPixelColor(8, ((red_values[22] + red_values[23] + red_values[24])/ (3 * PREVIEWDIM)), ((green_values[22] + green_values[23] + green_values[24])/ (3 * PREVIEWDIM)), ((blue_values[22] + blue_values[23] + blue_values[24])/ (3 * PREVIEWDIM)));
  strip.show();

  
}

void reset_strand() {
  /* Start by initializing all pixels to white */
  for(i=0;i<=TOPLED;i++) {
    red_values[i]=0;
    green_values[i]=0;
    blue_values[i]=0;
  }
  update_strand();
}


void randomize_colors() {
  RED = random(0,256);
  BLUE = random(0,256);
  GREEN = random(0,256);
}


void CheckSwitches() {

  while (digitalRead(TCL_MOMENTARY1)==LOW && digitalRead(TCL_MOMENTARY2)==LOW) { // The button is active (closed)
    leo_mode();
  }   

  while (digitalRead(TCL_MOMENTARY1)==LOW && digitalRead(TCL_MOMENTARY2)==HIGH) { 
    int i;
    for(i=0;i<=TOPLED;i++) {
      red_values[i]=255;
      green_values[i]=255;
      blue_values[i]=255;
    }
    update_strand(); // Send the black pixels to the strand to turn off all LEDs.
    delay(50);
    reset_strand();
    delay(50);
  }
  
  while (digitalRead(TCL_MOMENTARY1)==HIGH && digitalRead(TCL_MOMENTARY2)==LOW) { 
    int i;
    for(i=0;i<=TOPLED;i++) {
      red_values[i]=255;
      green_values[i]=255;
      blue_values[i]=0;
    }
    update_strand(); // Send the black pixels to the strand to turn off all LEDs.
    delay(50);
    reset_strand();
    delay(50);
  }


  if (digitalRead(6) == 0 && digitalRead(7) == 0){
    SWITCHSTATE = 3;
  }
  else if (digitalRead(6) == 0 && digitalRead(7) == 1){
    SWITCHSTATE = 2;
  }
  else if (digitalRead(6) == 1 && digitalRead(7) == 0){
    SWITCHSTATE = 1;
  }
  else{
    SWITCHSTATE = 0;
  }
}



void cylon_eye() {
  int j; // The lag counter
  int pos;

  while ( SWITCHSTATE == 3) {

    // Forward color sweep
    for(i=0; i<=TOPLED;i++){
      check_color_pots();
      red_values[i]=RED;
      blue_values[i]=BLUE;
      green_values[i]=GREEN;
      for(j=1;j<=10;j++) {
        pos=i-j;
        if(pos>=0) {
          red_values[pos] = red_values[pos] / 2;
          green_values[pos] = green_values[pos] / 2;
          blue_values[pos] = blue_values[pos] / 2;
        }
      } 

      // Empty out all trailing LEDs.  This prevents 'orphans' when dynamically shortening the tail length.
      for(pos=i-j; pos>=0;pos--){
        red_values[pos]=0;
        green_values[pos]=0;
        blue_values[pos]=0;
      }

      update_strand(); // Send all the pixels out
      delay(map(analogRead(TCL_POT4), 0, 1023, DELAYLOW, DELAYHIGH));

      CheckSwitches();
      if ( 3 != SWITCHSTATE ) {
        break;
      }
    }

    CheckSwitches();
    if ( 3 != SWITCHSTATE ) {
      break;
    }

    // Reverse color sweep
    for(i=TOPLED; i>=0;i--){
      check_color_pots();
      red_values[i]=RED;
      blue_values[i]=BLUE;
      green_values[i]=GREEN;
      for(j=1;j<=10;j++) {
        pos=i+j;
        if(pos<=TOPLED) {
          red_values[pos] = red_values[pos] / 2;
          green_values[pos] = green_values[pos] / 2;
          blue_values[pos] = blue_values[pos] / 2;
        }
      }

    // Empty out all trailing LEDs.  This prevents 'orphans' when dynamically shortening the tail length.
      for(pos=i+j; pos<=TOPLED;pos++){
        red_values[pos]=0;
        green_values[pos]=0;
        blue_values[pos]=0;
      }

      update_strand(); // Send all the pixels out
      delay(map(analogRead(TCL_POT4), 0, 1023, DELAYLOW, DELAYHIGH));

      CheckSwitches();
      if ( 3 != SWITCHSTATE ) {
        break;
      }
    }
    
  }
  reset_strand();
}

void check_color_pots() {
    /* Read the current red value from potentiometer 0 */
    RED=map(analogRead(TCL_POT1), 0, 1023, 0, 255);
    /* Read the current green value from potentiometer 1 */
    GREEN=map(analogRead(TCL_POT2), 0, 1023, 0, 255);
    /* Read the current blue value from potentiometer 2 */
    BLUE=map(analogRead(TCL_POT3), 0, 1023, 0, 255);

    int colorSUM = (RED + BLUE + GREEN);
    ratioRED = ( (float)RED / colorSUM );
    ratioBLUE = ( (float)BLUE / colorSUM );
    ratioGREEN = ( (float)GREEN / colorSUM );
    
    ratioHIGHEST = ratioRED;
    if ( ratioHIGHEST < ratioBLUE ) {
      ratioHIGHEST = ratioBLUE;
    }
    if ( ratioHIGHEST < ratioGREEN ) {
      ratioHIGHEST = ratioGREEN;
    }
    


}



void hippie_catcher() {
  int MIDCOUNT = (int(LEDCOUNT/2));
  randomize_colors();
    
  while ( SWITCHSTATE == 2) {
    for(i=0; i< MIDCOUNT; i++) {
      red_values[i]=red_values[(i+1)];
      blue_values[i]=blue_values[(i+1)];
      green_values[i]=green_values[(i+1)];
      CheckSwitches();
      if ( 2 != SWITCHSTATE ) {
        break;
      }
    }
    for(i=TOPLED; i>MIDCOUNT; i--) {
      red_values[i]=red_values[(i-1)];
      blue_values[i]=blue_values[(i-1)];
      green_values[i]=green_values[(i-1)];
      CheckSwitches();
      if ( 2 != SWITCHSTATE ) {
        break;
      }
    }  

    red_values[MIDCOUNT]=RED;
    blue_values[MIDCOUNT]=BLUE;
    green_values[MIDCOUNT]=GREEN;

    update_strand(); // Send all the pixels out
    delay(map(analogRead(TCL_POT4), 0, 1023, DELAYLOW, DELAYHIGH));
    RED = RED + red_modifier;
    GREEN = GREEN + green_modifier;
    BLUE = BLUE + blue_modifier;

    if (RED <= 0){
      RED = 0;
      red_modifier = red_modifier * -1;
    }
    if (RED >= 255){
      RED = 255;
      red_modifier = red_modifier * -1;
    }
  
    if (BLUE <= 0){
      BLUE = 0;
      blue_modifier = blue_modifier * -1;
    }
    if (BLUE >= 255){
      BLUE = 255;
      blue_modifier = blue_modifier * -1;
    }
  
    if (GREEN <= 0){
      GREEN = 0;
      green_modifier = green_modifier * -1;
    }
    if (GREEN >= 255){
      GREEN = 255;
      green_modifier = green_modifier * -1;
    }


    CheckSwitches();
  }

  reset_strand();

}


void leo_mode() {
  int MIDCOUNT = (int(LEDCOUNT/2));
  int i;
  int j;  

  for(j=1; j <= LEOBLINK; j++){
    if ( !(digitalRead(TCL_MOMENTARY1)==LOW && digitalRead(TCL_MOMENTARY2)==LOW)) { break;}  // Let's break the loop if the buttons are released
    for(i=0; i< MIDCOUNT; i++) {
      red_values[i]=255;
      blue_values[i]=0;
      green_values[i]=0;
    }
    
      red_values[MIDCOUNT]=0;
      blue_values[MIDCOUNT]=0;
      green_values[MIDCOUNT]=0;
    
    for(i=TOPLED; i>MIDCOUNT; i--) {
      red_values[i]=0;
      blue_values[i]=255;
      green_values[i]=0;
    }  
    update_strand(); // Send all the pixels out
    delay(LEODELAY);

    for(i=0; i<=TOPLED; i++) {
      red_values[i]=0;
      blue_values[i]=0;
      green_values[i]=0;
    }
    update_strand(); // Send all the pixels out
    delay(LEODELAY);
  }

  for(j=1; j <= LEOBLINK; j++){
    if ( !(digitalRead(TCL_MOMENTARY1)==LOW && digitalRead(TCL_MOMENTARY2)==LOW)) { break;}  // Let's break the loop if the buttons are released
    for(i=0; i< MIDCOUNT; i++) {
      red_values[i]=0;
      blue_values[i]=255;
      green_values[i]=0;
    }

      red_values[MIDCOUNT]=0;
      blue_values[MIDCOUNT]=0;
      green_values[MIDCOUNT]=0;

    for(i=TOPLED; i>MIDCOUNT; i--) {
      red_values[i]=255;
      blue_values[i]=0;
      green_values[i]=0;
    }  
    update_strand(); // Send all the pixels out
    delay(LEODELAY);

    for(i=0; i<=TOPLED; i++) {
      red_values[i]=0;
      blue_values[i]=0;
      green_values[i]=0;
    }
    update_strand(); // Send all the pixels out
    delay(LEODELAY);
  }

  reset_strand();

}


void constant_light() {

  while ( SWITCHSTATE == 0) {
    check_color_pots();

    for(i=0;i<=TOPLED;i++) {
      red_values[i]=RED;
      green_values[i]=GREEN;
      blue_values[i]=BLUE;
    }
    update_strand();
    CheckSwitches();
  }
  reset_strand();
}



void pulsing_light() {
  int j;


  
  while ( SWITCHSTATE == 1) {

    for( j=int(64/ratioHIGHEST); j <= int(255/ratioHIGHEST); j++) {
      check_color_pots();
      for(i=0;i<=TOPLED;i++) {
        red_values[i]=int(j * ratioRED);
        green_values[i]=int(j * ratioGREEN);
        blue_values[i]=int(j * ratioBLUE);
      }
      update_strand();
      CheckSwitches();
      if ( 1 != SWITCHSTATE ) { break; }
      delay(int(map(analogRead(TCL_POT4), 0, 1023, DELAYLOW, DELAYHIGH) / 10));
    }

    for( j=int(255/ratioHIGHEST); j >= int(64/ratioHIGHEST); j--) {
      check_color_pots();
      for(i=0;i<=TOPLED;i++) {
        red_values[i]=int(j * ratioRED);
        green_values[i]=int(j * ratioGREEN);
        blue_values[i]=int(j * ratioBLUE);
      }
      update_strand();
      CheckSwitches();
      if ( 1 != SWITCHSTATE ) { break; }
      delay(int(map(analogRead(TCL_POT4), 0, 1023, DELAYLOW, DELAYHIGH) / 10));
    }


  }
  
  
  reset_strand();
}
