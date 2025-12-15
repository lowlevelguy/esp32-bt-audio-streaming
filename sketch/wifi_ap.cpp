#include <WiFi.h>

const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

IPAddress open_wifi() {
  WiFi.softAP(ssid, password);

  return WiFi.softAPIP();
}


