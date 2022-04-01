#include <CapacitiveSensor.h>
/*
   CapitiveSense Library Demo Sketch
   Paul Badger 2008
   Uses a high value resistor e.g. 10M between send pin and receive pin
   Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
   Receive pin is the sensor pin - try different amounts of foil/metal on this pin
*/
// 10M resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4, 2);
CapacitiveSensor   cs_4_6 = CapacitiveSensor(4, 6);
CapacitiveSensor   cs_4_8 = CapacitiveSensor(4, 8);

CapacitiveSensor   cs_1 = CapacitiveSensor(12, 10);
CapacitiveSensor   cs_2 = CapacitiveSensor(14, 16);
CapacitiveSensor   cs_3 = CapacitiveSensor(22, 26);
CapacitiveSensor   cs_4 = CapacitiveSensor(30, 34);
CapacitiveSensor   cs_5 = CapacitiveSensor(38, 42);
CapacitiveSensor   cs_6 = CapacitiveSensor(46, 50);

#define CapacitiveSensor8_threshold 1780 - 300
#define CapacitiveSensor9_threshold 1900 - 300
#define CapacitiveSensor7_threshold 11000
#define CS1 (9400+11100)/2
#define CS2 (7000+10700)/2
#define CS3 (8400+13500)/2
#define CS4 (8300+13400)/2
#define CS5 (7100+9000)/2
#define CS6 (5000+6600)/2

int state = 0;

//LED Set up
#include <FastLED.h>

#define NUM_STRIPS 6
#define NUM_LEDS_PER_STRIP 20
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define BRIGHTNESS  60
#define FRAMES_PER_SECOND 60
bool gReverseDirection = false;
int r = 0;
int g = 0;
int b = 0;

long lastMillis = 0;
long currentMillis = 0;
int fin = 0;
int fin_out = 0;

void setup()
{
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(9600);

  //setup LED
  FastLED.addLeds<LED_TYPE, 13, COLOR_ORDER>(leds[0], 5).setCorrection(TypicalLEDStrip);//page 1 (pride)
  FastLED.addLeds<LED_TYPE, 11, COLOR_ORDER>(leds[1], 4).setCorrection(TypicalLEDStrip);//page 2 (pride)
  FastLED.addLeds<LED_TYPE, 9, COLOR_ORDER>(leds[2], 4).setCorrection(TypicalLEDStrip);//page 2 (pride)
  FastLED.addLeds<LED_TYPE, 7, COLOR_ORDER>(leds[3], 20).setCorrection(TypicalLEDStrip);//page 3 (ocean)
  FastLED.addLeds<LED_TYPE, 5, COLOR_ORDER>(leds[4], 2).setCorrection(TypicalLEDStrip);//page 4 (glooming yellow)
  FastLED.addLeds<LED_TYPE, 3, COLOR_ORDER>(leds[5], 5).setCorrection(TypicalLEDStrip);//page 6

  FastLED.setBrightness(BRIGHTNESS);
  Serial.println(F("LED Set up done.\n"));

}

void loop()
{
  
  long start = millis();
  long total7 =  cs_4_2.capacitiveSensor(30);
  long total8 =  cs_4_6.capacitiveSensor(30);
  long total9 =  cs_4_8.capacitiveSensor(30);

  long total1 =  cs_1.capacitiveSensor(30);
  long total2 =  cs_2.capacitiveSensor(30);
  long total3 =  cs_3.capacitiveSensor(30);
  long total4 =  cs_4.capacitiveSensor(30);
  long total5 =  cs_5.capacitiveSensor(30);
  long total6 =  cs_6.capacitiveSensor(30);

  Serial.print(millis() - start);        // check on performance in milliseconds
  Serial.print("\t");                    // tab character for debug windown spacing
  Serial.print(total1); Serial.print("\t");
  Serial.print(total2); Serial.print("\t");
  Serial.print(total3); Serial.print("\t");                   // tab character for debug windown spacing
  Serial.print(total4); Serial.print("\t");
  Serial.print(total5); Serial.print("\t");
  Serial.print(total6); Serial.print("\t");
  Serial.print(total7); Serial.print("\t");
  Serial.print(total8); Serial.print("\t");
  Serial.println(total9);                // print sensor output 3
  delay(10);

  if (total1 < CS1 && total2 < CS2 && total3 < CS3 && total4 < CS4 && total5 < 1000 && total6 < 1000 && state > 1) {
    state = 0;
  }
  
  // Page :   1   2   3c  4c  5   6
  // State: 0-1-  2   3   5
  //                  4

  switch (state) {

    //Page 0: program begins,playing 1st video
    case 0:
      Serial.println(0);
      state = 1;
      clear_led();
      break;

    //Page 1, waiting a flip
    case 1:
      pride(state);

      if (total2 > CS2) {//flip to page 2
        Serial.println(1);
        state = 2;
        clear_led();
      }
      break;

    //Page 2, waiting a flip
    case 2:
      pride(state);

      if (total3 > CS3) {//flip to page 3
        Serial.println(2);
        state = 3;
        clear_led();
      }
      break;

    //Page 3, waiting a flip
    //Page 3, waiting a touch to state 4
    case 3:

      if (total4 > CS4) {//flip to page 4
        Serial.println(3);
        state = 5;
        clear_led();
        r = 255;
        g = 255;
        b = 0;//yellow for next page
      }
      if (total8 > CapacitiveSensor8_threshold) {
        Serial.println(1);
        state = 4;
        clear_led();
      }
      break;
    case 4:
      pacifica_loop();
      if (total4 > CS4) {//flip to page 4
        Serial.println(3);
        state = 5;
        clear_led();
        r = 255;
        g = 255;
        b = 0;//yellow for next page
      }
      if (total8 < CapacitiveSensor8_threshold) {
        Serial.println(2);
        state = 3;
        clear_led();
      }
      break;

    //Page 4, frightening, waiting a flip
    //Page 4, waiting a touch to state 6, no go back
    case 5:
      glooming_yellow();
      leds[4][1] = CRGB(r, g, b);
      // Serial.println(r);
      // Serial.println(g);
      if (total5 > CS5) {//flip to page 4
        Serial.println(5);
        state = 7;
        clear_led();
      }
      if (total9 > CapacitiveSensor9_threshold) {
        Serial.println(4);
        state = 6;
        clear_led();
      }
      break;
    case 6:
      //fix the LEDs to yellow
      leds[4][1] = CRGB(255, 255, b);
      if (total5 > CS5) {//flip to page 4
        Serial.println(5);
        state = 7;
        clear_led();
      }
      break;


    //Page 5, waiting a flip to
    case 7:
      if (total6 > CS6) {//flip to page 3
        Serial.println(6);
        state = 8;
        clear_led();
        r = 255;
        g = 0;
        b = 0;
      }
      break;

    //Page 6, waiting a flip
    //Page 6, waiting a touch to state 9
    case 8:
      //5 fingers
      glooming_red();
      for (int i = 0; i < 5; i++) {
        leds[5][i] = CRGB(r, g, b);
      }

      fin_out = finger(total7);
      for (int i = 0; i < fin_out; i++) {
        leds[5][i] = CRGB(255, 255, b);
      }
      Serial.println(fin_out);
      if (fin_out > 5) {
        Serial.println(7);
        state = 9;
        clear_led();
      }
      break;
    case 9:
      for (int i = 0; i < 5; i++) {
        leds[5][i] = CRGB(255, 255, b);
      }
      break;
      
    default:
      break;
  }

  FastLED.show();

}

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride(int state)
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < NUM_LEDS_PER_STRIP; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV( hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS_PER_STRIP - 1) - pixelnumber;

    if (state == 1) {
      nblend( leds[0][pixelnumber], newcolor, 64);
    }
    if (state == 2) {
      nblend( leds[1][pixelnumber], newcolor, 64);
      nblend( leds[2][pixelnumber], newcolor, 64);
    }

  }
}

//////////////////////////////////////////////////////////////////////////
//
// The code for this animation is more complicated than other examples, and
// while it is "ready to run", and documented in general, it is probably not
// the best starting point for learning.  Nevertheless, it does illustrate some
// useful techniques.
//
//////////////////////////////////////////////////////////////////////////
//
// In this animation, there are four "layers" of waves of light.
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then
// another filter is applied that adds "whitecaps" of brightness where the
// waves line up with each other more.  Finally, another pass is taken
// over the led array to 'deepen' (dim) the blues and greens.
//
// The speed and scale and motion each layer varies slowly within independent
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//
CRGBPalette16 pacifica_palette_1 =
{ 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
  0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50
};
CRGBPalette16 pacifica_palette_2 =
{ 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
  0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F
};
CRGBPalette16 pacifica_palette_3 =
{ 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33,
  0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF
};

void pacifica_loop()
{
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
  sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
  sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
  sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( leds[3], NUM_LEDS_PER_STRIP, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0 - beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10, 38), 0 - beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10, 28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for ( uint16_t i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[3][i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );

  for ( uint16_t i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[3][i].getAverageLight();
    if ( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[3][i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for ( uint16_t i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds[3][i].blue = scale8( leds[3][i].blue,  145);
    leds[3][i].green = scale8( leds[3][i].green, 200);
    leds[3][i] |= CRGB( 2, 5, 7);
  }
}

void clear_led() {
  for (int x = 0; x < NUM_STRIPS; x++) {
    for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
      leds[x][i] = CRGB::Black;
    }
  }
}

int glomming_index = 9;
int gloom = 0;
void glooming_yellow() {
  if (gloom == 1) {
    r = r + glomming_index;
    g = g + glomming_index;
  } else {
    r = r - glomming_index;
    g = g - glomming_index;
  }

  if (r < 20) {
    gloom = 1;
  }
  if (r > 255 - glomming_index - 1) {
    gloom = 0;
  }
}

void glooming_red() {
  if (gloom == 1) {
    r = r + glomming_index;
  } else {
    r = r - glomming_index;
  }
  if (r < 20) {
    gloom = 1;
  }
  if (r > 255 - glomming_index - 1) {
    gloom = 0;
  }
}

int finger(int total7) {
  currentMillis = millis();
  if (currentMillis - lastMillis >= 2000) {
    lastMillis = currentMillis;
    if (total7 > CapacitiveSensor7_threshold) {
      fin++;
    } else {
      fin = 0;
    }
  }
  return fin;
}
