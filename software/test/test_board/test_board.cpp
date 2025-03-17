/*
  By: AnyFridge Team
  Term: Spring 2025
 
  This example demonstrates how to get the board functional.

*/

#include <Arduino.h>
#include "HardwareSerial.h"
#include "unity.h"

int ledPin = 13;

// ---------------------------------------------------------------------------
// Test 1: Serial print to confirm connectivity
// ---------------------------------------------------------------------------
void test_serial(){
    TEST_ASSERT_TRUE(Serial.println("Hello, world!"));
}

// ---------------------------------------------------------------------------
// Arduino Setup & Loop for Unity Tests
// ---------------------------------------------------------------------------
void setup()
{
    pinMode(ledPin, OUTPUT);
    Serial.begin(115200);

    // Wait for USB seems to not work
    while (!Serial) {
        delay(100); // Wait for native USB
    }

    // Extra 5-sec delay to allow the user to open the Serial Monitor
    delay(5000);

    // Initialize Unity:
    UNITY_BEGIN();

    // Run Tests:
    RUN_TEST(test_serial);

    // Finish Unity and do not rerun:
    UNITY_END();
}

void loop()
{
    Serial.println("Hello, world!");
    
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
}