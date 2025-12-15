#include "webserver.h"
#include "bluetooth.h"  // Assuming bluetooth functions exist
#include <WiFi.h>

// Global reference to bluetooth devices (from bluetooth.cpp)
extern String* btDevices;
extern int btDeviceCount;

void handleRootPage(AsyncWebServerRequest* request) {
    // Scan for devices
    scanBluetoothDevices();  // From bluetooth.cpp
    
    // Build HTML page with device list
    String html = "<!DOCTYPE html><html><head>";
    html += "<title>Select Bluetooth Speaker</title>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<style>";
    html += "body{font-family:Arial;padding:20px;text-align:center;}";
    html += "a{display:block;padding:15px;margin:10px;background:#007bff;color:white;text-decoration:none;border-radius:5px;}";
    html += "</style>";
    html += "</head><body>";
    html += "<h1>Select Bluetooth Speaker</h1>";
    
    if (btDeviceCount == 0) {
        html += "<p>No devices found. Rescanning...</p>";
        html += "<script>setTimeout(()=>location.reload(),2000)</script>";
    } else {
        for (int i = 0; i < btDeviceCount; i++) {
            html += "<a href='/connect?id=" + String(i) + "'>" + btDevices[i] + "</a>";
        }
    }
    
    html += "</body></html>";
    
    request->send(200, "text/html", html);
}

void handleConnectPage(AsyncWebServerRequest* request) {
    String message;
    
    if (request->hasParam("id")) {
        int id = request->getParam("id")->value().toInt();
        
        if (id >= 0 && id < btDeviceCount) {
            // Connect to device
            connectToBluetoothDevice(id);  // From bluetooth.cpp
            message = "<h1>Connected to " + btDevices[id] + "!</h1>";
            message += "<p>System will reboot in streaming mode.</p>";
        } else {
            message = "<h1>Invalid device ID</h1>";
        }
    } else {
        message = "<h1>Missing device ID</h1>";
    }
    
    message += "<p><a href='/'>Back</a></p>";
    
    request->send(200, "text/html", message);
}

void setupWebServer(AsyncWebServer* server) {
    // Only two routes - NO LittleFS, NO CSS, NO JS
    server->on("/", HTTP_GET, handleRootPage);
    server->on("/connect", HTTP_GET, handleConnectPage);
    
    server->begin();
    Serial.println("Web server started (minimal mode)");
}