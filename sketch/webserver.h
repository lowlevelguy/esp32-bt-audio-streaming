#ifndef ESP32BTAUDIO_WEBSERVER
#define ESP32BTAUDIO_WEBSERVER

#include <WebServer.h>

void setupWebServer(WebServer* server);
void handleRoot();
void handleConnect();

#endif