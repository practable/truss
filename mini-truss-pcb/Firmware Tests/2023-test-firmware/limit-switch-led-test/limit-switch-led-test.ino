#define BUTTON_PIN 11
#define LED_PIN 13
#define ENABLE_PIN A1


#include <buttonObject.h>
#include <ledObject.h>

ledObject led(LED_PIN);
buttonObject button(BUTTON_PIN, BUTTON_PULL_LOW);

void setup() {
  Serial.begin(115200);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);
  button.begin();
  led.begin();
  led.callBlink(10, 100, 200);
}


void loop() {
  button.buttonLoop();

  //bool buttonState = button.sampleButton();

  //Serial.println(buttonState);

  if (button.shortPress) {
    led.callBlink(2, 100, 200);
    button.buttonReset();
  }

  if (button.longPress) {
    led.callBlink(6, 100, 200);
  }

  led.performBlink();
  Serial.println(button.pressCount);
}
