#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Fonts/FreeSans9pt7b.h>

#define i2c_Address 0x3C

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Logos
#define LOGO_HEIGHT 8
#define LOGO_WIDTH  8
static const unsigned char PROGMEM logo_master[] =
{ 0x30, 0x77, 0xf0, 0xf7, 0xf0, 0xf7, 0x70, 0x30
};
static const unsigned char PROGMEM logo_edge[] =
{ 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c
};
static const unsigned char PROGMEM logo_discord[] =
{ 0xe7, 0xff, 0x99, 0x99, 0xff, 0xff, 0xff, 0xc3
};
static const unsigned char PROGMEM logo_mic[] =
{ 0x18, 0x3c, 0x3c, 0x3c, 0x3c, 0x18, 0x18, 0x3c
};
static const unsigned char PROGMEM logo_juegos[] =
{ 0x3c, 0xff, 0xdb, 0xbd, 0xe7, 0xff, 0xe7, 0x42
};
                                       
const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A8, A9};

int actualVolume[NUM_SLIDERS];
int analogSliderValues[NUM_SLIDERS];
String analogSliderNames[NUM_SLIDERS] = {"MASTER","EDGE","DISCORD","MIC","JUEGOS"};

const unsigned long sleepAfter = 500;
unsigned long startTime;
unsigned long currTime;
bool standby = 0;

int first = NUM_SLIDERS;

void setup() { 
  //Serial.begin(9600);
  Serial1.begin(9600);

  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
  }
  Serial.print("Pines iniciados");
  if(!display.begin(i2c_Address, true)) {
    //Serial.println(F("SH1106 allocation failed"));
    Serial1.println(F("SH1106 allocation failed"));
    for(;;);
  }
  startTime = millis();
}

void loop() {

  updateSliderValues();
  sendSliderValues();

  updateOLED();

  delay(10);
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    analogSliderValues[i] = (analogRead(analogInputs[i]) - 1023) * -1;

    if (abs(analogSliderValues[i] - actualVolume[i]) > 30 && first == 0) {
      actualVolume[i] = analogSliderValues[i];
      displayVol(i);
      display.display();
      startTime = currTime;
      standby = 0;
    }else if(abs(analogSliderValues[i] - actualVolume[i]) > 30 && first >= 0){
      --first;
      actualVolume[i] = analogSliderValues[i];
      startTime = currTime;
      standby = 0;
    }
  }
}

void sendSliderValues() {
  String builtString = String(""); 

  for (int i = 0; i < NUM_SLIDERS; i++) {
      builtString += String((int)actualVolume[i]);
      if (i < NUM_SLIDERS - 1) {
        builtString += String("|");
      }
    }
  //Serial.println(builtString);
  Serial1.println(builtString);
}

void updateOLED(){
  currTime = millis();
  if (standby == 0 && (currTime - startTime >= sleepAfter)) {
   alwayson();
   standby = 1;
   display.display();
   startTime = currTime;
  }
}

void alwayson(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  for(int i = 0; i < NUM_SLIDERS; ++i){
    display.fillRect(13, 5+(i*12), (analogSliderValues[i] / 13.5), 7, SH110X_WHITE);  
    display.setCursor(0, 4+(i*12));
    switch (i) {
      case 0:
        display.drawBitmap(2, 4+(i*12), logo_master, LOGO_WIDTH, LOGO_HEIGHT, SH110X_WHITE);
        break;
      case 1:
        display.drawBitmap(2, 4+(i*12), logo_edge, LOGO_WIDTH, LOGO_HEIGHT, SH110X_WHITE);
        break;
      case 2:
        display.drawBitmap(2, 4+(i*12), logo_discord, LOGO_WIDTH, LOGO_HEIGHT, SH110X_WHITE);
        break;
      case 3:
        display.drawBitmap(2, 4+(i*12), logo_mic, LOGO_WIDTH, LOGO_HEIGHT, SH110X_WHITE);
        break;
      case 4:
        display.drawBitmap(2, 4+(i*12), logo_juegos, LOGO_WIDTH, LOGO_HEIGHT, SH110X_WHITE);
        break;
      default:
        break;
    }
    display.setCursor(92, 4+(i*12));
    display.print(percentage_volume(analogSliderValues[i]));
    display.setCursor(116, 4+(i*12));
    display.print("%");
    display.drawFastHLine(12, 4+(i*12), 76, SH110X_WHITE);
    display.drawFastHLine(12, 12+(i*12), 76, SH110X_WHITE);
    display.drawFastVLine(12, 4+(i*12), 9, SH110X_WHITE);
    display.drawFastVLine(88, 4+(i*12), 9, SH110X_WHITE);
  }
}

void displayVol(int i){
  int percentage = percentage_volume(analogSliderValues[i]);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.fillRect(0, 2, percentage*1.28, 10, SH110X_WHITE);
  display.setCursor(0, 20);
  display.println(analogSliderNames[i]);
  display.setCursor(0, 40);
  display.print(percentage);
  display.setCursor(38, 40);
  display.print("%");
  display.display();
}

int percentage_volume(int actual_value){
  return (actual_value) / 10.2;
}