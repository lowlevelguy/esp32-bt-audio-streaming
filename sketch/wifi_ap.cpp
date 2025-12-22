#include "wifi_ap.h"

const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

IPAddress wifi_init() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  return WiFi.softAPIP();
}


