#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>

#include "led.h"
#include "network_config.h"
#include "http_control.h"

const char* LOCAL_EXT = "local"; // Usually 'local'

// HTTP firmware update
const char* update_path = "/update";
ESP8266HTTPUpdateServer httpUpdater;

WiFiUDP udp;
const unsigned int localUdpPort = 4210;
char incomingPacket[180];

// trigger
// max brightness
// min brightness
// total fade time

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

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  setup_leds();
  set_hostname();
  setup_wifi();
  setup_mdns();
  setup_http_server();
  setup_http_update_server(); // Must happen after HTTP server is setup

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
    if (len > 0) {
      incomingPacket[len] = 0;
    }

    Serial.printf("UDP packet contents: %s\n", incomingPacket);
  }
}
