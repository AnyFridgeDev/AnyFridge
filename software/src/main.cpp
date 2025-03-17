/*
  By: AnyFridge Team (some code sourced from hoshmandent - https://github.com/hoshmandent/POST-request-Arduino)
  Term: Spring 2025 
*/

#include <Arduino.h>
#include <ArduinoJson.h>
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
const char *host        = "172.20.10.4";
const int hostPort      = 5000;

enum mode {ADDITION, SUBTRACTION};

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

}

bool post_code(char *code, mode mode)
{
    WiFiClient client;
    bool connected = client.connect(host, hostPort);

    if (!connected) {
        Serial.println("Connection to server failed");
        return false;
    }

    // Define the URL and the payload of the POST request
    String endpoint = "/update";
    String user = "test_user";
    String payload;
    String action;

    if (mode == ADDITION) action = "POST";
    else action = "DELETE";

    JsonDocument doc;
    doc["upc_code"] = String(code);
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

    // Debugging output: display the response from the server
    Serial.println("Response:");
    while(client.available()){
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
    
    Serial.println();

    // Close the connection
    client.stop();

    return true;
}

void loop()
{ 
    scanner.startScan();
    delay(500);
    if (scanner.readBarcode(scanBuffer, BUFFER_LEN))
    {
        Serial.print("Code found: ");
        Serial.print(String(scanBuffer));
        Serial.println();
        post_code(scanBuffer, ADDITION);
    }
    scanner.stopScan();
  
    delay(500);
}