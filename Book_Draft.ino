#include <CapacitiveSensor.h>
/*
   CapitiveSense Library Demo Sketch
   Paul Badger 2008
   Uses a high value resistor e.g. 10M between send pin and receive pin
   Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
   Receive pin is the sensor pin - try different amounts of foil/metal on this pin
*/
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4, 2);       // 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_4_6 = CapacitiveSensor(4, 6);       // 10M resistor between pins 4 & 6, pin 6 is sensor pin, add a wire and or foil
CapacitiveSensor   cs_4_8 = CapacitiveSensor(4, 8);       // 10M resistor between pins 4 & 8, pin 8 is sensor pin, add a wire and or foil
long total[3] = {0, 0, 0};
#define CapacitiveSensor1_threshold 300
#define CapacitiveSensor2_threshold 100
#define CapacitiveSensor3_threshold 100

// constants won't change. They're used here to set pin numbers:
const int buttonPin[] = {9, 7, 5, 3, 1}; // the number of the pushbutton pin
const int buttonCount = 5;
// variables will change:
int buttonState[5] = {0, 0, 0, 0, 0};     // variable for reading the pushbutton status
int state = 0;

//LED Set up
#include <FastLED.h>

#define NUM_STRIPS 4
#define NUM_LEDS_PER_STRIP 3
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define BRIGHTNESS  60
#define FRAMES_PER_SECOND 60
bool gReverseDirection = false;
int r = 0;
int g = 0;
int b = 0;

void setup()
{
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(9600);

  for (int thisButton = 0; thisButton < buttonCount; thisButton++)  {
    pinMode(buttonPin[thisButton], OUTPUT);
  }

  //setup LED
  FastLED.addLeds<LED_TYPE, 13, COLOR_ORDER>(leds[0], 3).setCorrection(TypicalLEDStrip);//page 1
  FastLED.addLeds<LED_TYPE, 12, COLOR_ORDER>(leds[1], 1).setCorrection(TypicalLEDStrip);//page 2
  FastLED.addLeds<LED_TYPE, 11, COLOR_ORDER>(leds[2], 1).setCorrection(TypicalLEDStrip);//page 2
  FastLED.addLeds<LED_TYPE, 10, COLOR_ORDER>(leds[3], 1).setCorrection(TypicalLEDStrip);//page 3

  FastLED.setBrightness(BRIGHTNESS);
  Serial.println(F("LED Set up done.\n"));

}

void loop()
{

  buttonState[0] = digitalRead(buttonPin[0]);
  buttonState[1] = digitalRead(buttonPin[1]);
  buttonState[2] = digitalRead(buttonPin[2]);
  buttonState[3] = digitalRead(buttonPin[3]);
  buttonState[4] = digitalRead(buttonPin[4]);

  long total1 =  cs_4_2.capacitiveSensor(30);
  long total2 =  cs_4_6.capacitiveSensor(30);
  long total3 =  cs_4_8.capacitiveSensor(30);

  if (buttonState[0] == HIGH && buttonState[1] == HIGH && buttonState[2] == HIGH && buttonState[3] == HIGH && state > 2) {
    state = 0;
  }

  switch (state) {

    //program begins,playing 1st video
    case 0:
      Serial.println(0);
      state = 1;
      break;

    //stage 1, waiting a flip to stage 2
    case 1:
      for (int i = 0; i < 3; i++) {
        leds[0][i] = CRGB::Blue;
      }

      if (buttonState[0] == HIGH && buttonState[1] == LOW && buttonState[2] == HIGH) {
        Serial.println(1);
        state = 2;
        clear_led();
        r = 255;
      }
      break;

    //stage 2, waiting a flip to stage 3
    case 2:
      glooming_green();
      for (int i = 0; i < 1; i++) {
        leds[1][i] = CRGB(r, g, b);
        leds[2][i] = CRGB(0, 255, 255);
      }

      if (buttonState[1] == HIGH && buttonState[2] == LOW) {
        Serial.println(2);
        state = 3;
        clear_led();
        r = 255;
        g = 255;
        b = 0;
      }
      break;

    //stage 3, waiting a flip to stage 4
    //stage 3, waiting a touch to stage 2.1
    case 3:

      glooming_yellow();
      leds[3][0] = CRGB(r, g, b);
      /*
Serial.print(r);
Serial.print("\t");
Serial.println(g);
*/
      if (buttonState[2] == HIGH && buttonState[3] == LOW) {
        Serial.println(3);
        state = 5;
        clear_led();
      }
      if (total1 > CapacitiveSensor1_threshold) {
        Serial.println(1);
        state = 4;
        clear_led();
      }
      break;

    case 4:
      leds[3][0] = CRGB(255, 255, b);

      if (buttonState[2] == HIGH && buttonState[3] == LOW) {
        Serial.println(3);
        state = 5;
        clear_led();
      }
      if (total1 < CapacitiveSensor1_threshold) {
        Serial.println(2);
        state = 3;
        clear_led();
      }
      break;

    default:
    break;
  }
  FastLED.show();



  //Serial.println(total1);                  // print sensor output 1
  /*
    Serial.print(total2);                  // print sensor output 2
    Serial.print("\t");
    Serial.println(total3);                // print sensor output 3

    delay(10);                             // arbitrary delay to limit data to serial port
  */
}

void clear_led() {
  for (int x = 0; x < NUM_STRIPS; x++) {
    for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
      leds[x][i] = CRGB::Black;
    }
  }
}

void glooming_green() {
  if (r > 0 && b == 0) {
    r--;
    g++;
  }
  if (g > 0 && r == 0) {
    g--;
    b++;
  }
  if (b > 0 && g == 0) {
    r++;
    b--;
  }
}

int gloom = 0;
void glooming_yellow() {
  if (gloom == 1) {
    r++;
    g++;
  } else {
    r--;
    g--;
  }

  if (r < 20) {
    gloom = 1;
  }
  if (r > 255) {
    gloom = 0;
  }
}
