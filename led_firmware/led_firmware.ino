#include <stdbool.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>

#include "led.h"
#include "network_config.h"
#include "http_control.h"
#include "offsets.h"

const char* LOCAL_EXT = "local"; // Usually 'local'

// HTTP firmware update
const char* update_path = "/update";
ESP8266HTTPUpdateServer httpUpdater;

WiFiUDP udp;
const unsigned int localUdpPort = 4210;
// Two bytes per LED
char incomingPacket[180 * 2];
int my_led_value_offset;

//============ SETUP

// Set a unique hostname with a common prefix
void set_hostname() {
  deviceHostname = HOSTNAME_PREFIX;
  deviceHostname += String(ESP.getChipId(), HEX);
}

void setup_wifi() {
  WiFi.hostname(deviceHostname);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_NAME, SSID_PWD);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(2000);
    ESP.restart();
  }
}

void setup_mdns() {
  if (!MDNS.begin(deviceHostname.c_str())) {
    Serial.println("Error setting up MDNS responder!");

    while (1) {
      delay(1000);
    }
  }

  MDNS.addService("http", "tcp", 80);
}

void setup_http_update_server() {
  httpUpdater.setup(&server, update_path);
}

void setup_udp() {
  udp.begin(localUdpPort);
}

void set_offset() {
  uint32_t id = ESP.getChipId();

  for (int i = 0; i < sizeof(ESP_ADDRESS_OFFSETS) / sizeof(uint32_t); i++) {
    if (ESP_ADDRESS_OFFSETS[i] == id) {
      // Straighten the zig-zag strand pattern into a regular raster pattern
      int y = i / 10;
      bool leftToRight = y % 2 == 0;

      if (leftToRight) {
        my_led_value_offset = i;
      } else {
        my_led_value_offset = y * 10 + 10 - (i - y * 10) - 1;
      }

      return;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting UDP receiver");

  setup_leds();
  set_hostname();
  Serial.println("Setting up WiFi");
  setup_wifi();
  Serial.println("WiFi set up");
  setup_mdns();
  Serial.println("MDNS set up");
  setup_http_server();
  setup_http_update_server(); // Must happen after HTTP server is setup
  Serial.println("HTTP set up");
  setup_udp();
  Serial.println("UDP set up");

  set_offset();

  Serial.printf("HTTPServer ready! Open http://%s.%s in your browser\n", deviceHostname.c_str(), LOCAL_EXT);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();

  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());

    int len = udp.read(incomingPacket, sizeof(incomingPacket));

    //if (len == sizeof(incomingPacket)) {
      // This is an LED update
      uint16_t brightnessH = incomingPacket[my_led_value_offset * 2];
      uint16_t brightnessL = incomingPacket[my_led_value_offset * 2 + 1]; 
      float brightness = ((brightnessH << 8) | brightnessL) / (float)0x3ff;

      set_brightness(brightness);
    //}
  }

  http_control_update();
}
