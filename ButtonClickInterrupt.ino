/***********************************

This ESP32 code shows how to use Interrupt Service Routines (ISRs) to detect button clicks.
Two buttons are used to pull-down the voltage on GPIO pins 12 and 14.
The ESP32 has configurable, internal PULLUP and PULLDOWN resistors that are used with the buttons.  
Note that the internal resistors are ONLY available on GPIO pins GPIO 0 thru GPIO 33. 

This code is designed to safely handle switch bouncing.

NOTES ON ISRs
-------------
An Interrupt Service Routine (ISR) can be triggered based on different conditions of a digital signal. 
For GPIO pins, these conditions often relate to the pin's voltage transitioning between low and high states.

Using the Arduino framework for the ESP32, when you attach an interrupt to a pin using 
  attachInterrupt()

You can also specify the condition for which the ISR will be triggered:

RISING: The ISR is triggered when the pin transitions from LOW to HIGH.
  attachInterrupt(digitalPinToInterrupt(pinNumber), ISRFunction, RISING);

FALLING: The ISR is triggered when the pin transitions from HIGH to LOW.
  attachInterrupt(digitalPinToInterrupt(pinNumber), ISRFunction, FALLING);

CHANGE: The ISR is triggered on any change in pin state (either from LOW to HIGH or from HIGH to LOW).
  attachInterrupt(digitalPinToInterrupt(pinNumber), ISRFunction, CHANGE);

LOW: The ISR is triggered when the pin is in a LOW state. This is level-triggered, meaning as long as the pin remains low, the interrupt remains active. 
This mode may not be available or behave the same way on all platforms, so check the specifics for the ESP32.
  attachInterrupt(digitalPinToInterrupt(pinNumber), ISRFunction, LOW);

ISR CODE EXAMPLES
-----------------
Here are examples of how you can set up an ESP32 GPIO pin with an interrupt and specify the internal resistor type using the Arduino framework.
These are just basic examples to demonstrate the usage of the internal pull-up and pull-down resistors alongside interrupts on the ESP32. 

1)  Using an Internal Pull-Up Resistor with a Falling Edge Interrupt:
    This would be appropriate for a button that connects the pin to ground when pressed:

const int pinNumber = 2;  // Replace with your pin number

void setup() {
    pinMode(pinNumber, INPUT_PULLUP);  
    attachInterrupt(digitalPinToInterrupt(pinNumber), ISRFunction, FALLING);
}

void ISRFunction() {
    // Your interrupt code here
}

void loop() {
    // Main loop code here
}

2)  Using an Internal Pull-Down Resistor with a Rising Edge Interrupt:
    This could be used for a button that connects the pin to VCC (3.3V for ESP32) when pressed:

const int pinNumber = 2;  // Replace with your pin number

void setup() {
    pinMode(pinNumber, INPUT_PULLDOWN);  
    attachInterrupt(digitalPinToInterrupt(pinNumber), ISRFunction, RISING);
}

void ISRFunction() {
    // Your interrupt code here
}

void loop() {
    // Main loop code here
}

3)  Using an Internal Pull-Up Resistor with a Change Interrupt:
    This will trigger the ISR on any transition, either HIGH to LOW or LOW to HIGH. Useful when you need to detect both press and release of a button:

const int pinNumber = 2;  // Replace with your pin number

void setup() {
    pinMode(pinNumber, INPUT_PULLUP);  
    attachInterrupt(digitalPinToInterrupt(pinNumber), ISRFunction, CHANGE);
}

void ISRFunction() {
    // Your interrupt code here
}

void loop() {
    // Main loop code here
}

***********************************/

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
