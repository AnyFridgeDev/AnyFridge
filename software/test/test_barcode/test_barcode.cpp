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
#include "HardwareSerial.h"
#include "unity.h"
HardwareSerial barcodeSerial(1); // UART BUS 1

#include "SparkFun_DE2120_Arduino_Library.h" 
DE2120 scanner;

#define BUFFER_LEN 40
char scanBuffer[BUFFER_LEN];

// ---------------------------------------------------------------------------
// Test 1: Scanner initialization
// ---------------------------------------------------------------------------
void test_init(){
    TEST_ASSERT_TRUE(scanner.begin(barcodeSerial));
}

// ---------------------------------------------------------------------------
// Test 2: Scanner connectivity check
// ---------------------------------------------------------------------------
void test_connected(){
    TEST_ASSERT_TRUE(scanner.isConnected());
}

// ---------------------------------------------------------------------------
// Test 3: Scanner reticle toggle
// ---------------------------------------------------------------------------
void test_reticle(){
    TEST_ASSERT_TRUE(scanner.reticleOff());
    delay(500);
    TEST_ASSERT_TRUE(scanner.reticleOn());
}

// ---------------------------------------------------------------------------
// Test 3: Scanner light toggle
// ---------------------------------------------------------------------------
void test_light(){
    TEST_ASSERT_TRUE(scanner.lightOn());
    delay(500);
    TEST_ASSERT_TRUE(scanner.lightOff());
}

// ---------------------------------------------------------------------------
// Arduino Setup & Loop for Unity Tests
// ---------------------------------------------------------------------------
void setup()
{
    Serial.begin(115200);

    // Wait for USB seems to not work
    while (!Serial) {
        delay(100); // Wait for native USB
    }

    // Extra 5-sec delay to allow the user to open the Serial Monitor
    delay(5000);

    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_connected);
    RUN_TEST(test_reticle);
    RUN_TEST(test_light);
    UNITY_END();
}

void loop()
{
    scanner.startScan();
    delay(500);
    if (scanner.readBarcode(scanBuffer, BUFFER_LEN))
    {
        Serial.print("Code found: ");
        for (int i = 0; i < strlen(scanBuffer); i++)
        Serial.print(scanBuffer[i]);
        Serial.println();
    }
    scanner.stopScan();
  
    delay(500);
}