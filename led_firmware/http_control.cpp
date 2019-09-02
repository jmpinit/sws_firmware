#include "Arduino.h"
#include <ESP8266mDNS.h>
#include "network_config.h"
#include "http_control.h"

#include "led.h"

ESP8266WebServer server(80);
String deviceHostname;

// Brightness values are between 0 and 1
int blinkDuration = 100; // ms
int fadeDuration = 100; //ms

float minBrightness = 0;
float maxBrightness = 1;

bool blinking = false;
long blinkStartTime;

bool fading = false;
bool fadeDown = false;
long fadeStartTime;

// 404
void route_not_found() {
  String message = "";
  message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  // Print out the query string parameters
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void route_hostname() {
  server.send(200, "text/plain", deviceHostname);
}

void route_mdns_query() {
  Serial.println("Sending mDNS query");

  int n = MDNS.queryService("http", "tcp"); // Send out query for esp tcp services
  String mdnsres = "";

  Serial.println("mDNS query done");

  if (n == 0) {
    Serial.println("no services found");
  } else {
    for (uint8_t i = 0; i < n; i++) {
      Serial.println(MDNS.hostname(i));

      // Save name of each valid hostname found - based on HOSTNAME prefix
      if (strncmp(MDNS.hostname(i).c_str(), HOSTNAME_PREFIX, 9) == 0) {
        mdnsres += MDNS.hostname(i);
        mdnsres += "\n";
      }
    }

    server.send(200, "text/plain", mdnsres);
  }

  Serial.println("mDNS Query Results:");
  Serial.println(mdnsres);
}

void route_blink() {
  if (server.hasArg("MIN")) {
    float newMinBrightness = constrain(atoi(server.arg("MIN").c_str()), 0, 1023) / 1023.0;

    if (newMinBrightness < maxBrightness) {
      minBrightness = newMinBrightness;
    }
  }

  if (server.hasArg("MAX")) {
    float newMaxBrightness = constrain(atoi(server.arg("MAX").c_str()), 0, 1023) / 1023.0;

    if (newMaxBrightness > minBrightness) {
      maxBrightness = newMaxBrightness;
    }
  }

  if (server.hasArg("BLINK_DURATION")) {
    float newBlinkDuration = (long)atoi(server.arg("BLINK_DURATION").c_str());
    float MAX_BLINK_DURATION = 10001;
    if (newBlinkDuration > 0 && newBlinkDuration < MAX_BLINK_DURATION) {
      blinkDuration = newBlinkDuration;
    }
  }

  blinking = true;
  blinkStartTime = millis();

  String message = "Blinking for ";
  message += blinkDuration;
  message += " ms from ";
  message += minBrightness * 100;
  message += "% to ";
  message += maxBrightness * 100;
  message += "% brightness.";

  server.send(200, "text/plain", message);
  Serial.println(message);
}

// Handles route to fade in/out led
//  'ipaddress/fade?FROM=0&TO=1023&DURATION=10000'

void route_fade() {
  float newFromBrightness;
  float newToBrightness;

  if (server.hasArg("FROM")) {
    newFromBrightness = constrain(atoi(server.arg("FROM").c_str()), 0, 1023) / 1023.0;
  }

  if (server.hasArg("TO")) {
    newToBrightness = constrain(atoi(server.arg("TO").c_str()), 0, 1023) / 1023.0;
  }


  if (newFromBrightness > newToBrightness) {
    fadeDown = true;

    maxBrightness = newFromBrightness;
    minBrightness = newToBrightness;
  }
  else {
    fadeDown = false;

    maxBrightness = newToBrightness;
    minBrightness = newFromBrightness;
  }

  if (server.hasArg("DURATION")) {
    float newFadeDuration = (long)atoi(server.arg("DURATION").c_str());
    float MAX_DURATION = 10000;
    if (newFadeDuration > 0 && newFadeDuration < MAX_DURATION) {
      fadeDuration = newFadeDuration;
    }
  }

  fading = true;
  fadeStartTime = millis();

  String message = "Fading for ";
  message += fadeDuration;
  message += " ms, from ";
  message += newFromBrightness * 100;
  message += "% to ";
  message += newToBrightness * 100;
  message += "% brightness.";

  server.send(200, "text/plain", message);
  Serial.println(message);
}

void route_off() {
  server.send(200, "text/plain", "LED off");
  blinking = false;
  fading = false;
  set_brightness(0);
}

// Handles /
void route_root() {
  server.send(200, "text/plain", "hello - 7/9/2019");
}

void setup_http_server() {
  // Create routes
  server.on("/", route_root);
  server.on("/hostname", route_hostname);
  server.on("/mdnsquery", route_mdns_query);
  server.on("/blink", route_blink);
  server.on("/fade", route_fade);
  server.on("/off", route_fade);
  server.onNotFound(route_not_found);

  server.begin();
}

// A blink is from max brightness to min brightness and back again
void blink(float rawRatioComplete) {
  float ratioComplete = constrain(rawRatioComplete, 0, 1);

  // % of the range between min and max brightness
  float brightnessRatio = (ratioComplete < 0.5) ?
                          (1 - ratioComplete * 2) :
                          ((ratioComplete - 0.5) * 2);

  // The actual brightness to set
  float brightness = brightnessRatio * (maxBrightness - minBrightness) + minBrightness;

  set_brightness(brightness);
}

// A fade is from one value to another value, either going up or down
void fade(float rawRatioComplete) {
  float brightnessRatio = constrain(rawRatioComplete, 0, 1);

  //if fading down, inverse the brightness ratio
  if (fadeDown == true) {
    (brightnessRatio = (-brightnessRatio) + 1.0);
  }

  // The actual brightness to set
  float brightness = brightnessRatio * (maxBrightness - minBrightness) + minBrightness;
  set_brightness(brightness);
}

void http_control_update() {
  long currentTime = millis();

  if (fading) {
    long fadeEndTime = fadeStartTime + fadeDuration;

    if (currentTime > fadeEndTime) {
      // Stop blinking
      fading = false;
    } else {
      float fadeRatio = (currentTime - fadeStartTime) / (float)fadeDuration;
      fade(fadeRatio);
    }
  }

  if (blinking) {
    long blinkEndTime = blinkStartTime + blinkDuration;

    if (currentTime > blinkEndTime) {
      // Stop blinking
      blinking = false;
    } else {
      float blinkRatio = (currentTime - blinkStartTime) / (float)blinkDuration;
      blink(blinkRatio);
    }
  }
}
