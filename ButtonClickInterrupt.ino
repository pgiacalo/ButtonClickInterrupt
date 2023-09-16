/*
 * This ESP32 code shows how to use Interrupt Service Routines (ISRs) to detect button clicks.
 * Two buttons are used to pull-down the voltage on GPIO pins 12 and 14.
 * The code is designed to safely handle switch bouncing.
 */

#include <Arduino.h>

#define BUTTON_UP_PIN     12          // GPIO pin (note that GPIO pins 0 thru 33 have pullup resistors)
#define BUTTON_DOWN_PIN   14          // GPIO pin (note that GPIO pins 0 thru 33 have pullup resistors)

#define DEBUG             true        // Controls serial output messages 

const int debounceDelayMillis = 10;              // time to wait to avoid switch/button bouncing

volatile bool upPressed = false;      // Flag for button press (volatile for changes by interrupt)
volatile bool downPressed = false;    // Flag for button press (volatile for changes by interrupt)

volatile unsigned long upInterruptTime = 0;
volatile unsigned long upLastInterruptTime = 0;

volatile unsigned long downInterruptTime = 0;
volatile unsigned long downLastInterruptTime = 0;

int counter = 0;                      // Global counter for button presses
const int minCount = 1;               // minimum limit for circular counting
const int maxCount = 10;              // maximum limit for circular counting

void setup() {
  Serial.begin(115200);
  initializeButtons();
}

void loop() {
  checkForButtonPress();

  //do other stuff here
}

void initializeButtons(){
  upLastInterruptTime = millis();
  upInterruptTime = upLastInterruptTime;

  downLastInterruptTime = millis();
  downInterruptTime = downLastInterruptTime;

  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_UP_PIN), buttonUpISR, FALLING);

  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_DOWN_PIN), buttonDownISR, FALLING);
}

void checkForButtonPress(){
  if (upPressed) {
    upInterruptTime = millis();
    if (upInterruptTime - upLastInterruptTime > debounceDelayMillis){  
      if (digitalRead(BUTTON_UP_PIN) == LOW) {
        counter++;  // Increment the counter
        //enforce circular counting
        if (counter > maxCount){
          counter = minCount;
        }
        if (DEBUG){
          Serial.println(counter);
        }
        upLastInterruptTime = upInterruptTime;
      }
      
      while (digitalRead(BUTTON_UP_PIN) == LOW);  // Wait until the button is released

      upPressed = false;

      if (DEBUG){
        Serial.println("Up button released");
      }

    }    
  }

  if (downPressed) {
    downInterruptTime = millis();
    if (downInterruptTime - downLastInterruptTime > debounceDelayMillis){  
      if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
        counter--;  // Decrement the counter
        //enforce circular counting
        if (counter < minCount){
          counter = maxCount;
        }
        if (DEBUG){
          Serial.println(counter);
        }
        downLastInterruptTime = downInterruptTime;
      }
      
      while (digitalRead(BUTTON_DOWN_PIN) == LOW);  // Wait until the button is released

      downPressed = false;

      if (DEBUG){
        Serial.println("Down button released");
      }

    }    
  }  
}

// Interrupt Service Routine
void buttonUpISR() {
  upPressed = true;
  upInterruptTime = millis();
  upLastInterruptTime = upInterruptTime;
}

// Interrupt Service Routine
void buttonDownISR() {
  downPressed = true;
  downInterruptTime = millis();
  downLastInterruptTime = downInterruptTime;
}
