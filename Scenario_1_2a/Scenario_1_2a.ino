#include <CapacitiveSensor.h>
#include <FastLED.h>
#include <ezButton.h>

//DEFINING LED PINS AND VALUES
#define LED_PIN 5
#define NUM_LEDS 52
int red, green, blue;
int idleLED, connectionLED, responseLED, anxietyLED;

//DEFINING BUTTON PINS AND VALUES
#define onButton 11
#define sendButton 12
#define lightButton 6
#define moderateButton 7
#define severeButton 8
int onState, sendState, light, moderate, severe, breathingExercise;

//DEFINING CAPACITIVE PINS AND VALUES
#define CSendPin 4
#define CReceivePin 2
long capacitanceVal;
int anxiety_mode;

//DEFINING MISC VALUES
char *mood;
unsigned long counter, counterMax;

//INITIALIZATION
CRGB leds[NUM_LEDS];
ezButton toggleswitch(onButton);
ezButton sendswitch(sendButton);
ezButton lightMode(lightButton);
ezButton moderateMode(moderateButton);
ezButton severeMode(severeButton);
CapacitiveSensor capacitive = CapacitiveSensor(CSendPin, CReceivePin);


void setup() {
  //Start Up Delay
  delay(1000);

  //LED SETUP
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(50, 50, 50);
  }
  FastLED.show();

  //SERIAL SETUP
  Serial.begin(9600);

  //BUTTON DEBOUNCE
  toggleswitch.setDebounceTime(50); // set debounce time to 50 milliseconds
  sendswitch.setDebounceTime(50);
  lightMode.setDebounceTime(50);
  moderateMode.setDebounceTime(50);
  severeMode.setDebounceTime(50);

  //INT VALUE SETUP
  onState = LOW;
  sendState = LOW;
  anxiety_mode = LOW;
  breathingExercise = LOW;
  red = 0;
  green = 0;
  blue = 0;
  counter = 0;
  counterMax = 2000 + random(1000, 2000);

  //INITIAL FUNCTION TO START
  delay(2000);
  Serial.println("SYSTEM RUNNING");
  delay(2000);
  Serial.println(counterMax);
  delay(2000);

  for (int i = 0; i < 20; i++) {
    Serial.println(capacitive.capacitiveSensor(30));
    delay (10);
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
}

//==============================================================================
void loop() {
  toggleswitch.loop();
  long capacitanceVal =  capacitive.capacitiveSensor(30);
  Serial.println(capacitanceVal);
  if (toggleswitch.isPressed()) {
    onState = !onState;
    light = LOW;
    moderate = LOW;
    severe = LOW;
    sendState = LOW;
    mood = "  ";
    anxiety_mode = LOW;
    breathingExercise = LOW;
  }

  if (onState == HIGH) {
    connectionMode();
  }
  else if (capacitanceVal > 100) {
    anxiety_mode = HIGH;
  }
  else {
    idle();
    anxiety_mode = LOW;
  }
  displayLEDS();
  //Serial.println(capacitanceVal);
}
//==============================================================================

void connectionMode() {
  sendswitch.loop();
  lightMode.loop();
  moderateMode.loop();
  severeMode.loop();

  //BUTTON PRESSES
  if (lightMode.isPressed()) {
    light = HIGH;
    moderate = LOW;
    severe = LOW;
    mood = "light anxiety";
  }
  if (moderateMode.isPressed()) {
    light = LOW;
    moderate = HIGH;
    severe = LOW;
    mood = "moderate anxiety";
  }
  if (severeMode.isPressed()) {
    light = LOW;
    moderate = LOW;
    severe = HIGH;
    mood = "severe anxiety";
  }
  if (sendswitch.isPressed()) {
    sendState = !sendState;
  }

  //MOOD SELECTION
  if (light == HIGH) {
    red = 75;
    green = 75;
    blue = 75;
  }
  else if (moderate == HIGH) {
    red = 75;
    green = 75;
    blue = 0;
  }
  else if (severe == HIGH) {
    red = 255;
    green = 140;
    blue = 0;
  }
  else {
    red = 10;
    green = 10;
    blue = 10;
  }
  //RESET IF FALSE POSITIVE
  if (sendState == HIGH && (light == LOW && moderate == LOW && severe == LOW)) {
    sendState = LOW;
  }
}

void idle() {
  red = 0;
  green = 0;
  blue = 0;
}


void displayLEDS() {
  if (sendState == HIGH && (light == HIGH || moderate == HIGH || severe == HIGH)) {
    counter++;

    if (counter < counterMax) {
      uint8_t looper = beatsin8 (2560, 0, NUM_LEDS - 1, 0, 0);
      leds[looper] = CRGB(red, green, blue);
      fadeToBlackBy(leds, NUM_LEDS, 10);
    }
    else if ((counter > counterMax) && (counter < counterMax + 1000)) {
      EVERY_N_MILLISECONDS(20) {
        leds[random(0, NUM_LEDS - 1)] = CRGB(red, green, blue);
      }
      fadeToBlackBy(leds, NUM_LEDS, 10);
    }

    else if (counter > counterMax + 1000) {
      breathingExercise = HIGH;
      if (light == HIGH) {
        uint8_t redlight = beatsin8 (2560, 0, 75, 0, 0);
        uint8_t greenlight = beatsin8 (2560, 0, 75, 0, 0);
        uint8_t bluelight = beatsin8 (2560, 0, 75, 0, 0);
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(redlight, greenlight, bluelight);
        }
      }
      else if (moderate == HIGH) {
        uint8_t redlight = beatsin8 (2560, 0, 75, 0, 0);
        uint8_t greenlight = beatsin8 (2560, 0, 75, 0, 0);
        uint8_t bluelight = beatsin8 (2560, 0, 0, 0, 0);
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(redlight, greenlight, bluelight);
        }
      }
      else if (severe == HIGH) {
        uint8_t redlight = beatsin8 (2560, 0, 255, 0, 0);
        uint8_t greenlight = beatsin8 (2560, 0, 140, 0, 0);
        uint8_t bluelight = beatsin8 (2560, 0, 0, 0, 0);
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(redlight, greenlight, bluelight);
        }
      }
    }

  }
  else if (anxiety_mode == HIGH) {
    uint32_t referencepoint = 0;
    uint8_t anxietypattern = beatsin8(2560, 0, 255, referencepoint, 0);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(anxietypattern, anxietypattern, anxietypattern);
    }
    //Serial.println(anxietypattern);
  }
  else {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(red, green, blue);
    }
    counter = 0;
  }
  FastLED.show();
}
