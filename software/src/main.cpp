/*
  By: AnyFridge Team
  Term: Spring 2025 

  References:
  - POST example from https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/examples/BasicHttpsClient/BasicHttpsClient.ino
  - ISR from https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/
  - NetWizard example from https://docs.netwizard.pro/docs/intro/example
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <NetWizard.h>
#include <WebServer.h>

// This is a Google Trust Services cert, the root Certificate Authority that
// signed the server certificate for the demo server https://jigsaw.w3.org in this
// example. This certificate is valid until Jan 28 00:00:42 2028 GMT
const char *rootCACertificate = R"string_literal(
-----BEGIN CERTIFICATE-----
MIIDejCCAmKgAwIBAgIQf+UwvzMTQ77dghYQST2KGzANBgkqhkiG9w0BAQsFADBX
MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE
CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIzMTEx
NTAzNDMyMVoXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT
GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFI0
MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE83Rzp2iLYK5DuDXFgTB7S0md+8Fhzube
Rr1r1WEYNa5A3XP3iZEwWus87oV8okB2O6nGuEfYKueSkWpz6bFyOZ8pn6KY019e
WIZlD6GEZQbR3IvJx3PIjGov5cSr0R2Ko4H/MIH8MA4GA1UdDwEB/wQEAwIBhjAd
BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDwYDVR0TAQH/BAUwAwEB/zAd
BgNVHQ4EFgQUgEzW63T/STaj1dj8tT7FavCUHYwwHwYDVR0jBBgwFoAUYHtmGkUN
l8qJUC99BM00qP/8/UswNgYIKwYBBQUHAQEEKjAoMCYGCCsGAQUFBzAChhpodHRw
Oi8vaS5wa2kuZ29vZy9nc3IxLmNydDAtBgNVHR8EJjAkMCKgIKAehhxodHRwOi8v
Yy5wa2kuZ29vZy9yL2dzcjEuY3JsMBMGA1UdIAQMMAowCAYGZ4EMAQIBMA0GCSqG
SIb3DQEBCwUAA4IBAQAYQrsPBtYDh5bjP2OBDwmkoWhIDDkic574y04tfzHpn+cJ
odI2D4SseesQ6bDrarZ7C30ddLibZatoKiws3UL9xnELz4ct92vID24FfVbiI1hY
+SW6FoVHkNeWIP0GCbaM4C6uVdF5dTUsMVs/ZbzNnIdCp5Gxmx5ejvEau8otR/Cs
kGN+hr/W5GvT1tMBjgWKZ1i4//emhA1JG1BbPzoLJQvyEotc03lXjTaCzv8mEbep
8RqZ7a2CPsgRbuvTPBwcOMBBmuFeU88+FSBX6+7iP0il8b4Z0QFqIwwMHfs/L6K1
vepuoxtGzi4CZ68zJpiq1UvSqTbFJjtbD4seiMHl
-----END CERTIFICATE-----
)string_literal";

#include "HardwareSerial.h"
HardwareSerial barcodeSerial(1); // UART BUS 1

#include "SparkFun_DE2120_Arduino_Library.h" 
DE2120 scanner;

#define BUFFER_LEN 40
char scanBuffer[BUFFER_LEN] = {0};

// --- Configuration for AnyFridge endpoint ---
const char *host        = "https://af.ethananderson.dev/api/update";
String user             = "";

// --- NetWizard Configuration ---
WebServer server(80);
NetWizard NW(&server);
NetWizardParameter nw_header(&NW, NW_HEADER, "User Configuration");
NetWizardParameter nw_divider1(&NW, NW_DIVIDER);
NetWizardParameter nw_input(&NW, NW_INPUT, "User ID", "", "isaac"); 

// --- Network Communication ---
WiFiClientSecure *client = nullptr;
HTTPClient https;
uint32_t lastScanTime_ms = 0;
#define DISCONNECT_TIMEOUT_MS 10000 // After 10 seconds of no activity, disconnect from the server

enum mode {ADDITION, SUBTRACTION};
mode current_mode = ADDITION;

#define RED 33
#define GREEN 27
#define BLUE 12
#define BUTTON 15

unsigned long button_time = 0;
unsigned long last_button_time = 0;

void IRAM_ATTR button_isr() {

    button_time = millis();

    if (button_time - last_button_time > 250) {
        
        // Swap the current mode
        if (current_mode == ADDITION) {
            current_mode = SUBTRACTION;
            digitalWrite(RED, HIGH);
            digitalWrite(GREEN, LOW);
        } else {
            current_mode = ADDITION;
            digitalWrite(RED, LOW);
            digitalWrite(GREEN, HIGH);
        }
        last_button_time = button_time;
    }
    
}

bool connect_to_server(){
    client = new WiFiClientSecure();
    client->setCACert(rootCACertificate);
    bool connected = https.begin(*client, host);
    if (connected) {
        Serial.println("Connected to server");
    } else {
        Serial.println("Failed to connect to server");
    }
    return connected;
}

void setup()
{
    Serial.begin(115200);

    // Wait for USB seems to not work
    while (!Serial) {
        delay(100); // Wait for native USB
    }

    // Configure LEDS
    pinMode(RED, OUTPUT); // LED 1
    pinMode(GREEN, OUTPUT); // LED 2
    pinMode(BLUE, OUTPUT); // LED 3

    // Initialize scanner module
    while (!scanner.begin(barcodeSerial)){
        Serial.println("[-] Scanner not detected, retrying...");
        delay(1000);
    }

    Serial.println("[+] Scanner online!");

    scanner.lightOff();
    scanner.disableAll2D();
    scanner.reticleOn();
    scanner.stopScan();
    
    scanner.enableContinuousRead(1);
    scanner.enableMotionSense(50U);

    digitalWrite(BLUE, HIGH);

    // Resets NetWizard to default state
    // NW.reset();

    NW.setConnectTimeout(10000);
    NW.setStrategy(NetWizardStrategy::BLOCKING);
    NW.autoConnect("AnyFridge", "");

    Serial.println("[+] Connected to WiFi!");

    // If we are not providing a user, accept the user provided by NetWizard
    if (user.length() == 0) user = nw_input.getValueStr();

    digitalWrite(BLUE, LOW);
    digitalWrite(GREEN, HIGH);

    pinMode(BUTTON, INPUT_PULLDOWN);
    attachInterrupt(BUTTON, button_isr, FALLING);

    Serial.println("[+] AnyFridge ready!");

}

bool post_code(char *code)
{
    if (!https.connected()){
        Serial.println("[-] Not connected to server, trying to connect...");
        if (!connect_to_server()){
            Serial.println("[-] Failed to connect to server");
            return false;
        }
    }

    https.addHeader("Content-Type", "application/json");

    String payload;
    String action;

    noInterrupts();
    current_mode == ADDITION ? action = "POST" : action = "DELETE";
    interrupts();

    JsonDocument doc;
    doc["upc_code"] = String(code);
    doc["user_id"] = user;
    doc["action"] = action;
    serializeJson(doc, payload);

    int httpResponseCode = https.POST(payload);
    
    Serial.print(httpResponseCode);

    Serial.println();

    return true;
}

void loop()
{ 
    
    NW.loop();
    
    // Check if we need to disconnect from the server
    if (client && https.connected() && (millis() - lastScanTime_ms) > DISCONNECT_TIMEOUT_MS)
    {
        Serial.println("[-] Disconnecting from server");
        https.end();
        
        delete client;
        client = nullptr;
    }

    if (scanner.readBarcode(scanBuffer, BUFFER_LEN))
    {

        Serial.print("[+] Code found: ");
        Serial.println(String(scanBuffer));
        
        post_code(scanBuffer);

        // Flush scanBuffer and Serial line
        memset(scanBuffer, 0, BUFFER_LEN);
        while (barcodeSerial.available()) barcodeSerial.read();

        lastScanTime_ms = millis();

    }

}