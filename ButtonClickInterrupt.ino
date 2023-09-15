#include <Arduino.h>

#define BUTTON_UP_PIN     12    // GPIO pin (note GPIO pins 0 - 33 have pullup resistors)
#define BUTTON_DOWN_PIN   14    // GPIO pin (note GPIO pins 0 - 33 have pullup resistors)

volatile bool upPressed = false;    // Flag for button press (volatile for changes by interrupt)
volatile bool downPressed = false;  // Flag for button press (volatile for changes by interrupt)
int counter = 0;                    // Global counter for button presses

volatile unsigned long upInterruptTime = 0;
volatile unsigned long upLastInterruptTime = 0;

volatile unsigned long downInterruptTime = 0;
volatile unsigned long downLastInterruptTime = 0;

void setup() {
  Serial.begin(115200);
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
    if (upInterruptTime - upLastInterruptTime > 10){  //10 milliseconds for button bounce
      if (digitalRead(BUTTON_UP_PIN) == LOW) {
        counter++;  // Increment the counter
        Serial.println(counter);
        upLastInterruptTime = upInterruptTime;
      }
      
      while (digitalRead(BUTTON_UP_PIN) == LOW);  // Wait until the button is released

      upPressed = false;

    }    
  }

  if (downPressed) {
    downInterruptTime = millis();
    if (downInterruptTime - downLastInterruptTime > 10){  //10 milliseconds for button bounce
      if (digitalRead(BUTTON_DOWN_PIN) == LOW) {
        counter--;  // Decrement the counter
        Serial.println(counter);
        downLastInterruptTime = downInterruptTime;
      }
      
      while (digitalRead(BUTTON_DOWN_PIN) == LOW);  // Wait until the button is released

      downPressed = false;

    }    
  }  
}

void loop() {
  checkForButtonPress();

  //do other stuff here
}

void buttonUpISR() {
  upPressed = true;
  upInterruptTime = millis();
  upLastInterruptTime = upInterruptTime;
}

void buttonDownISR() {
  downPressed = true;
  downInterruptTime = millis();
  downLastInterruptTime = downInterruptTime;
}
