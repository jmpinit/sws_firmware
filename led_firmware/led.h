#ifndef H_LED
#define H_LED

const int BULB_PIN = 12; // The main external LED bulb
const int LED_PIN = 2; // The on-board LED

void setup_leds();
void set_brightness(float brightness);

#endif
