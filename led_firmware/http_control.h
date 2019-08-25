#ifndef H_ROUTES
#define H_ROUTES

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;
extern String deviceHostname;

void route_not_found();
void route_hostname();
void route_mdns_query();
void route_root();
void setup_http_server();
void http_control_update();

#endif
