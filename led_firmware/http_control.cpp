#include "Arduino.h"
#include <ESP8266mDNS.h>
#include "network_config.h"
#include "http_control.h"

ESP8266WebServer server(80);
String deviceHostname;

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

// Handles /
void route_root() {
  String localIP = String(WiFi.localIP());
  server.send(200, "text/plain", "hello");
}

void setup_http_server() {
  // Create routes
  server.on("/", route_root);
  server.on("/hostname", route_hostname);
  server.on("/mdnsquery", route_mdns_query);
  server.onNotFound(route_not_found);

  server.begin();
}
