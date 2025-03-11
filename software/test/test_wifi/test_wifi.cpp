#include <unity.h>
#include <Arduino.h>


void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(100); // wait for native usb
    }
    UNITY_BEGIN();

    UNITY_END();
}

void loop() {
    // Unity test framework doesn't use the loop function
}