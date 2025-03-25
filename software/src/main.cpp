/*
  By: AnyFridge Team (POST example referenced at https://github.com/espressif/arduino-esp32/blob/master/libraries/HTTPClient/examples/BasicHttpsClient/BasicHttpsClient.ino)
  Term: Spring 2025 
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

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
char scanBuffer[BUFFER_LEN];

// --- Replace these with your network credentials ---
const char *networkName = "iPhone";
const char *networkPswd = "password";

// --- Domain/Port for AnyFridge endpoint ---
const char *host        = "https://af.ethananderson.dev/api/update";
const String user       = "isaac";

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
    WiFiClientSecure *client = new WiFiClientSecure;
    HTTPClient http;

    client->setCACert(rootCACertificate);

    bool connected = http.begin(*client, host);

    if (!connected) {
        Serial.println("Connection to server failed");
        return false;
    }

    http.addHeader("Content-Type", "application/json");

    String payload;
    String action;

    if (mode == ADDITION) action = "POST";
    else action = "DELETE";

    JsonDocument doc;
    doc["upc_code"] = String(code);
    doc["user_id"] = user;
    doc["action"] = action;
    serializeJson(doc, payload);

    int httpResponseCode = http.POST(payload);
    
    Serial.print(httpResponseCode);

    Serial.println();

    // Close the connection
    http.end();

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