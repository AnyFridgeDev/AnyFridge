/*
  By: AnyFridge Team (some code sourced from hoshmandent - https://github.com/hoshmandent/POST-request-Arduino)
  Term: Spring 2025 
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <unity.h>
#include <WiFi.h>
#include "HardwareSerial.h"
HardwareSerial barcodeSerial(1); // UART BUS 1

#include "SparkFun_DE2120_Arduino_Library.h" 
DE2120 scanner;

#define BUFFER_LEN 40
char scanBuffer[BUFFER_LEN];

// --- Replace these with your network credentials ---
const char *networkName = "iPhone";
const char *networkPswd = "password";

// --- Domain/Port for AnyFridge endpoint ---
const char *host        = "";
const int hostPort      = 5000;

void test_integration()
{   
    // attempt to scan a code for up to ten seconds
    unsigned long startTime = millis();
    while (millis() - startTime < 10000)
    {
        scanner.startScan();
        delay(500);

        if (scanner.readBarcode(scanBuffer, BUFFER_LEN))
        {
            Serial.print("Code found: ");
            Serial.print(String(scanBuffer));
            Serial.println();
            
            WiFiClient client;
            bool connected = client.connect(host, hostPort);
        
            TEST_ASSERT_TRUE_MESSAGE(
                connected,
                "Failed to connect to server (client.connect() returned false)."
            );

            // Define the URL and the payload of the POST request
            String endpoint = "/update";
            String user = "test_user";
            String payload;
            String action;

            JsonDocument doc;
            doc["upc_code"] = String(scanBuffer);
            doc["user_id"] = user;
            doc["action"] = action;
            serializeJson(doc, payload);

            String request = ("POST " + endpoint + " HTTP/1.1\r\n" +
                            "Host: " + host + "\r\n" +
                            "Content-Type: application/json\r\n" +
                            "Content-Length: " + payload.length() + "\r\n" +
                            "Connection: close\r\n\r\n" +
                            payload);

            // Send the POST request
            Serial.print(request);
            Serial.println();
            
            client.print(request);

            // Close the connection
            client.stop();
            return;
        }
        scanner.stopScan();
        delay(500);
    }
    
    TEST_ASSERT_NOT_NULL_MESSAGE(NULL, "The scanner timed out.");
    
}

void setup()
{
    Serial.begin(115200);

    // Wait for USB seems to not work
    while (!Serial) {
        delay(100); // Wait for native USB
    }

    // Initialize scanner module
    scanner.begin(barcodeSerial);

    // Intitialize WiFi connectivity
    WiFi.mode(WIFI_STA);
    WiFi.begin(networkName, networkPswd);
  
    // Wait until successful connection
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(250);
    }

    // Assigned IP Address
    Serial.println(WiFi.localIP());

    // Extra 5-sec delay to allow the user to open the Serial Monitor
    delay(5000);

    // Initialize Unity:
    UNITY_BEGIN();

    // Run Tests:
    RUN_TEST(test_integration);

    // Finish Unity and do not rerun:
    UNITY_END();

}

void loop() {
    // Empty. Tests run once in setup().
}