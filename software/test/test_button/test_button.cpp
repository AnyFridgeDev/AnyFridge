/*
  By: AnyFridge Team (some code sourced from Nick Poole)
  Term: Spring 2025
 
  This example demonstrates how to get the scanner connected and will output any barcode it sees.

  To connect the barcode scanner to an Arduino:

  (Arduino pin) = (Scanner pin)
  2 = TX pin on scanner
  3 = RX pin on scanner
  GND = GND
  3.3V = 3.3V
*/

#include <Arduino.h>
#include "unity.h"

// ---------------------------------------------------------------------------
// Arduino Setup & Loop for Unity Tests
// ---------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);

    // // Wait for USB seems to not work
    // while (!Serial) {
    //     delay(100); // Wait for native USB
    // }

    // Configure the blink LEDs
    pinMode(33, OUTPUT); // LED 1
    pinMode(27, OUTPUT); // LED 2
    pinMode(12, OUTPUT); // LED 3

    // Configure the button GPIO on pin 15
    pinMode(15, INPUT); // Button GPIO

    delay(1000);
}

int presses = 0;

void loop()
{
    if (digitalRead(15) == HIGH) {
        presses++;

        // Set the LEDs based on the presses count
        digitalWrite(33, presses % 2 == 0 ? HIGH : LOW); // LED 1
        digitalWrite(27, presses % 4 == 0 ? HIGH : LOW); // LED 2
        digitalWrite(12, presses % 8 == 0 ? HIGH : LOW); // LED 3

        delay(500); // Debounce delay
    }
}