#include <Arduino.h>
#include <SevSeg.h>

/************************
 * Constants
 ************************/

static const uint8_t LED_PIN = 13;
static const uint8_t BUTTON_PIN = 14;

// 7-Segment Display Pins
static const uint8_t DISPLAY_1_PIN = 23;
static const uint8_t DISPLAY_2_PIN = 22;
static const uint8_t DISPLAY_3_PIN = 21;
static const uint8_t DISPLAY_4_PIN = 20;
static const uint8_t DISPLAY_A_PIN = 3;
static const uint8_t DISPLAY_B_PIN = 4;
static const uint8_t DISPLAY_C_PIN = 5;
static const uint8_t DISPLAY_D_PIN = 6;
static const uint8_t DISPLAY_E_PIN = 7;
static const uint8_t DISPLAY_F_PIN = 8;
static const uint8_t DISPLAY_G_PIN = 9;
static const uint8_t DISPLAY_DP_PIN = 10;

// Delay before accepting button state change for debouncing purposes
static const unsigned long LOCKOUT_DELAY_MILLIS = 100;

/************************
 * Variables
 ************************/
// True if the button has been pressed but not actioned
volatile bool gButtonStateChangeToAction = false;
// The debounced button state
uint8_t gButtonState = LOW;

// Seven Segment display instance
SevSeg mSevenSegmentDisplay;

// The system time that the light will next turn on after
unsigned long mSystemTimeOfNextLight = 0;
// The system time at which the light last turned on
unsigned long mLastLightOnTime = 0;

/*********************
 * Interrupt routines
 *********************/
void buttonPressedISR() {
  unsigned long currentMillis = millis();
  cli();
  static unsigned long lastButtonStateChangeTime = 0;
  uint8_t newButtonState = digitalReadFast(BUTTON_PIN);

  // Make sure we're not in the lockout period
  if (currentMillis - lastButtonStateChangeTime > LOCKOUT_DELAY_MILLIS) {
    // Only do anything if we've changed state
    if (gButtonState != newButtonState) {
      lastButtonStateChangeTime = currentMillis;
      gButtonStateChangeToAction = true;
      gButtonState = newButtonState;
    }
  }
  sei();
}

/*********************
 * Utility 
 *********************/
long generateNextLightTime() {
  return millis() + 1000 + random(5000);
}

/*********************
 * Entry Point methods
 *********************/
// Setup function - executes once on startup
void setup() {
  // Set up pins and internals
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);

  // Set up display
  byte numDigits = 4;
  byte digitPins[] = {DISPLAY_1_PIN, DISPLAY_2_PIN, DISPLAY_3_PIN,
                      DISPLAY_4_PIN};
  byte segmentPins[] = {DISPLAY_A_PIN, DISPLAY_B_PIN, DISPLAY_C_PIN,
                        DISPLAY_D_PIN, DISPLAY_E_PIN, DISPLAY_F_PIN,
                        DISPLAY_G_PIN, DISPLAY_DP_PIN};
  mSevenSegmentDisplay.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);

  // Set up Interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressedISR, CHANGE);

  // Other
  randomSeed(analogRead(0));
  mSystemTimeOfNextLight = generateNextLightTime();
}

// Loop function - loops :)
void loop() {
  
  // Turn the LED after a random period
  if (millis() > mSystemTimeOfNextLight && digitalRead(LED_PIN) == LOW) {
    digitalWrite(LED_PIN, HIGH);
    mLastLightOnTime = millis();
  }

  // Check to see if the user has pressed the button
  if (gButtonStateChangeToAction && gButtonState == HIGH) {
    long curTime = millis();
    // Turn the light off
    digitalWrite(LED_PIN, LOW);
    // Calculate the time difference and display it
    long timeDiff = curTime - mLastLightOnTime;
    mSevenSegmentDisplay.setNumber(timeDiff, 1);
    // Reset for next time
    mSystemTimeOfNextLight = generateNextLightTime();
    gButtonStateChangeToAction = false;
  }

  mSevenSegmentDisplay.refreshDisplay();
}