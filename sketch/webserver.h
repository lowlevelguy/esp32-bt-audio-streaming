#ifndef ESP32BTAUDIO_WEBSERVER
#define ESP32BTAUDIO_WEBSERVER

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

// ========================
// Web Server Public API
// ========================

/**
 * @brief Initialize and setup all web server routes
 * @param server Pointer to AsyncWebServer instance
 */
void setupWebServer(AsyncWebServer* server);

/**
 * @brief Set callbacks to Bluetooth module functions
 * @param scanCB Function to call when scanning is requested
 * @param connectCB Function to call when connecting to device
 * @param getDevicesCB Function to get JSON list of devices
 */
void setBluetoothCallbacks(
    void (*scanCB)(),
    void (*connectCB)(int deviceIndex),
    String (*getDevicesCB)()
);

/**
 * @brief Get WiFi AP IP address for display
 * @return String containing IP address
 */
String getWiFiIP();

#endif