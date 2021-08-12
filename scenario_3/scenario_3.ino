#include <CapacitiveSensor.h>
#include <FastLED.h>
#include <ezButton.h>

//DEFINING LED PINS AND VALUES
#define LED_PIN 5
#define NUM_LEDS 52
int red, green, blue;
int idleLED, connectionLED, responseLED, anxietyLED, pace;

//DEFINING BUTTON PINS AND VALUES
#define onButton 11
#define sendButton 12
#define slowButton 6
#define moderateButton 7
#define fastButton 8
int onState, sendState, slow, moderate, fast, breathingExercise;

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
ezButton slowMode(slowButton);
ezButton moderateMode(moderateButton);
ezButton fastMode(fastButton);
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
  slowMode.setDebounceTime(50);
  moderateMode.setDebounceTime(50);
  fastMode.setDebounceTime(50);

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
    slow = LOW;
    moderate = LOW;
    fast = LOW;
    sendState = LOW;
    mood = "  ";
    anxiety_mode = LOW;
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
}
//==============================================================================

void connectionMode() {
  sendswitch.loop();
  slowMode.loop();
  moderateMode.loop();
  fastMode.loop();

  //BUTTON PRESSES
  if (slowMode.isPressed()) {
    slow = HIGH;
    moderate = LOW;
    fast = LOW;
    mood = "slow breathing";
  }
  if (moderateMode.isPressed()) {
    slow = LOW;
    moderate = HIGH;
    fast = LOW;
    mood = "moderate breathing";
  }
  if (fastMode.isPressed()) {
    slow = LOW;
    moderate = LOW;
    fast = HIGH;
    mood = "fast breathing";
  }
  if (sendswitch.isPressed()) {
    sendState = !sendState;
  }

  //MOOD SELECTION
  if (slow == HIGH) {
    pace = 1536;
  }
  else if (moderate == HIGH) {
    pace = 2560;
  }
  else if (fast == HIGH) {
    pace = 2840;
  }
  else {
    pace = 2560;
  }
  //RESET IF FALSE POSITIVE
  if (sendState == HIGH && (slow == LOW && moderate == LOW && fast == LOW)) {
    sendState = LOW;
  }
}

void idle() {
  pace = 2560;
}


void displayLEDS() {

  if (onState == LOW && sendState == LOW && breathingExercise == LOW) {
    uint8_t looper = beatsin8 (2560, 0, NUM_LEDS - 1, 0, 0);
    leds[looper] = CRGB(100, 100, 0);
    fadeToBlackBy(leds, NUM_LEDS, 10);
    counter = 0;
  }
  else if (onState == HIGH && sendState == LOW) {
    uint32_t referencepoint = 0;
    uint8_t anxietypattern = beatsin8(pace, 0, 175, referencepoint, 0);
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(anxietypattern, anxietypattern, 0);
    }
  }

  else if (onState == HIGH && sendState == HIGH && (slow == HIGH || moderate == HIGH || fast == HIGH)) {
    counter++;

    if (counter < 300) {
      uint32_t referencepoint = 0;
      uint8_t anxietypattern = beatsin8(pace, 0, 175, referencepoint, 0);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, anxietypattern, 0);
      }
    }
    else if ((counter > 300) && (counter < counterMax + 1000)) {
      uint32_t referencepoint = 0;
      uint8_t anxietypattern = beatsin8(pace, 0, 175, referencepoint, 0);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(anxietypattern, anxietypattern, 0);
      }
    }
    else if (counter > counterMax + 1000 && counter < counterMax + 2500) {
      breathingExercise = HIGH;
      uint32_t referencepoint = 0;
      uint8_t anxietypattern = beatsin8(pace, 0, 175, referencepoint, 0);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(anxietypattern, anxietypattern, anxietypattern);
      }
    }
    else if (counter > counterMax + 2500) {
      EVERY_N_MILLISECONDS(20) {
        leds[random(0, NUM_LEDS - 1)] = CRGB(100, 100, 100);
      }
      fadeToBlackBy(leds, NUM_LEDS, 10);
    }
  }
  else if (onState == LOW && breathingExercise == HIGH) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
  }
  FastLED.show();
}
