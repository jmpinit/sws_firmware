#include "Arduino.h"
#include "led.h"

int brightnessLookup[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
  1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 6,
  6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11,
  11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16,
  17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23,
  23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31,
  31, 32, 33, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41,
  42, 43, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 54, 55, 56,
  57, 58, 60, 61, 63, 64, 66, 68, 69, 72, 74, 76, 79, 82, 86, 90,
  96, 103, 111, 121, 129, 137, 143, 148, 152, 157, 160, 164, 167, 171, 174, 177,
  180, 183, 186, 189, 192, 195, 198, 253, 251, 249, 247, 244, 240, 240, 240, 240,
};

void setup_leds() {
  pinMode(BULB_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  set_brightness(1);
}

// Brightness is 0 to 1 where 1 is brightest
void set_brightness(float brightness) {
  int brightnessIndex = (int)(0xff * constrain(brightness, 0, 1));
  int pwmDutyCycle = brightnessLookup[brightnessIndex] * (PWMRANGE / 256);

  analogWrite(BULB_PIN, pwmDutyCycle);
  analogWrite(LED_PIN, pwmDutyCycle);
}
