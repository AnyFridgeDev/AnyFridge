/*
  By: AnyFridge Team
  Term: Spring 2025
 
  This example demonstrates how to use the onboard WiFi functionality.

*/

#include <Arduino.h>
#include <unity.h>
#include <WiFi.h>

// --- Replace these with your network credentials ---
const char *networkName = "iPhone";
const char *networkPswd = "password";

// --- Domain/Port for your test request ---
const char *hostDomain = "www.google.com";
const int hostPort     = 80;

void test_wifi_connection(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(networkName, networkPswd);
  
  // Wait up to 10 seconds for connection:
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startAttemptTime) < 10000)
  {
    delay(250);
  }

  // Check final WiFi status:
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(
    WL_CONNECTED,
    WiFi.status(),
    "Failed to connect to WiFi within 10 seconds."
  );
}


void test_http_get_request(void)
{
  // Assume we are already connected to WiFi from previous test.
  WiFiClient client;
  bool connected = client.connect(hostDomain, hostPort);

  TEST_ASSERT_TRUE_MESSAGE(
    connected,
    "Failed to connect to server (client.connect() returned false)."
  );

  // If connected, send the GET request:
  client.print(String("GET / HTTP/1.1\r\n") +
               "Host: " + hostDomain + "\r\n" +
               "Connection: close\r\n\r\n");

  // Wait up to 5 seconds for data:
  unsigned long startTime = millis();
  while (!client.available() && millis() - startTime < 5000)
  {
    delay(100);
  }

  // Check if anything at all is available to read:
  int availableBytes = client.available();
  TEST_ASSERT_GREATER_THAN_MESSAGE(
    0,
    availableBytes,
    "No data received from server in 5 seconds."
  );

  // (Optional) Read out the first line or two for debugging:
  if (availableBytes > 0)
  {
    String line = client.readStringUntil('\n');
    Serial.println("Server response line: " + line);
    // If needed, you can parse or check the contents here.
  }

  // Done
  client.stop();
}

void test_any_fridge_website_ping(){
    // Hits af.ethananderson.dev with a GET request on port 8080
    // Should return a 200 OK status code

    // Assume we are already connected to WiFi from previous test.
    WiFiClient client;
    bool connected = client.connect("af.ethananderson.dev", 8080);

    TEST_ASSERT_TRUE_MESSAGE(
        connected,
        "Failed to connect to server (client.connect() returned false)."
    );

    // If connected, send the GET request:
    client.print(String("GET / HTTP/1.1\r\n") +
                "Host: af.ethananderson.dev\r\n" +
                "Connection: close\r\n\r\n");

    // Wait up to 5 seconds for data:
    unsigned long startTime = millis();
    while (!client.available() && millis() - startTime < 5000)
    {
        delay(100);
    }
    

    // Check if anything at all is available to read:
    int availableBytes = client.available();
    TEST_ASSERT_GREATER_THAN_MESSAGE(
        0,
        availableBytes,
        "No data received from server in 5 seconds."
    );

    // (Optional) Read out the first line or two for debugging:
    if (availableBytes > 0)
    {
        String line = client.readStringUntil('\n');
        Serial.println("Server response line: " + line);
        // If needed, you can parse or check the contents here.
    }

    // Done
    client.stop();
}


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
  RUN_TEST(test_wifi_connection);
  RUN_TEST(test_http_get_request);
  RUN_TEST(test_any_fridge_website_ping);

  // Finish Unity and do not rerun:
  UNITY_END();
}

void loop()
{

}
