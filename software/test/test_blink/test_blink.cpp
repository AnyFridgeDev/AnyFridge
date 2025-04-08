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

    delay(1000);
}

void loop()
{
    // Blink the LEDs on pins 33, 27, and 12
    digitalWrite(33, HIGH);
    digitalWrite(27, HIGH);
    digitalWrite(12, HIGH);
  
    delay(500);

    digitalWrite(33, LOW);
    digitalWrite(27, LOW);
    digitalWrite(12, LOW);

    delay(500);
}