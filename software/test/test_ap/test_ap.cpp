/*
  By: AnyFridge Team (some code sourced from Ankit Rana - https://gist.github.com/futechiot/ee0223dd269cbe7d8605ce97d120d7d2)
  Term: Spring 2025
 
  This example demonstrates how to put the ESP32 into AP mode.
*/

#include <Arduino.h>
#include <unity.h>
#include <WiFi.h>              

// --- Name advertised AP SSID ---
const char *apSSID     = "ESP32";
const char *expectedIP = "192.168.4.1";                

void test_ap(void) {
      
  // Changing ESP32 wifi mode to AP
  WiFi.mode(WIFI_AP);

  TEST_ASSERT_TRUE(WiFi.softAP(apSSID));      
  
  delay(250);

  // IP Address of our ESP32 AP
  // Default IP is 192.168.4.1
  IPAddress addr = WiFi.softAPIP();     
  TEST_ASSERT_EQUAL_STRING(expectedIP, addr.toString().c_str());             

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

    // Initialize Unity:
    UNITY_BEGIN();

    // Run Tests:
    RUN_TEST(test_ap);

    // Finish Unity and do not rerun:
    UNITY_END();
}

void loop()
{
  // Empty. Tests run once in setup().
}