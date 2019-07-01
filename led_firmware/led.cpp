#include "Arduino.h"
#include "led.h"

void setup_leds() {
  pinMode(BULB_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  set_brightness(1);
}

// Brightness is 0 to 1 where 1 is brightest
void set_brightness(float brightness) {
  int pwmDutyCycle = constrain(brightness, 0, 1) * PWMRANGE;

  analogWrite(BULB_PIN, pwmDutyCycle);
  analogWrite(LED_PIN, pwmDutyCycle);
}
