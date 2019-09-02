#include "Arduino.h"
#include "led.h"

void setup_leds() {
  pinMode(BULB_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  set_brightness(1);
}

// Brightness is 0 to 1 where 1 is brightest
void set_brightness(float brightness) {
  int brightnessIndex = (int)(0xff * constrain(brightness, 0, 1));
  int pwmDutyCycle = brightnessLookup[brightnessIndex] * PWMRANGE;

  analogWrite(BULB_PIN, pwmDutyCycle);
  analogWrite(LED_PIN, pwmDutyCycle);
}
