#ifndef ESP32BTAUDIO_WEBSERVER
#define ESP32BTAUDIO_WEBSERVER

#include <ESPAsyncWebServer.h>

// Only two functions needed
void setupWebServer(AsyncWebServer* server);
void handleRootPage(AsyncWebServerRequest* request);
void handleConnectPage(AsyncWebServerRequest* request);

#endif