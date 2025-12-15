#include "webserver.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// ========================
// Bluetooth Callbacks
// ========================
static void (*bluetoothScanCallback)() = nullptr;
static void (*bluetoothConnectCallback)(int) = nullptr;
static String (*bluetoothGetDevicesCallback)() = nullptr;

// ========================
// Public Functions
// ========================

void setBluetoothCallbacks(void (*scanCB)(), void (*connectCB)(int), String (*getDevicesCB)()) {
    bluetoothScanCallback = scanCB;
    bluetoothConnectCallback = connectCB;
    bluetoothGetDevicesCallback = getDevicesCB;
    Serial.println("[Web] Bluetooth callbacks registered");
}

String getWiFiIP() {
    return WiFi.softAPIP().toString();
}

void setupWebServer(AsyncWebServer* server) {
    Serial.println("[Web] Initializing web server...");
    
    // Initialize file system for web pages
    if (!LittleFS.begin(true)) {
        Serial.println("[Web] ERROR: LittleFS mount failed!");
        return;
    }
    Serial.println("[Web] LittleFS mounted successfully");
    
    // ========================
    // Static File Serving
    // ========================
    server->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    server->serveStatic("/style.css", LittleFS, "/style.css");
    server->serveStatic("/script.js", LittleFS, "/script.js");
    
    // ========================
    // API Endpoints
    // ========================
    
    // 1. Scan for Bluetooth devices
    server->on("/api/scan", HTTP_GET, [](AsyncWebServerRequest* request) {
        Serial.println("[Web] API: /api/scan called");
        
        if (!bluetoothScanCallback) {
            request->send(500, "application/json", "{\"error\":\"Bluetooth not initialized\"}");
            return;
        }
        
        // Execute Bluetooth scan
        bluetoothScanCallback();
        
        // Wait a bit for scan to complete
        delay(1500);
        
        // Return device list
        if (bluetoothGetDevicesCallback) {
            String devices = bluetoothGetDevicesCallback();
            request->send(200, "application/json", devices);
        } else {
            request->send(200, "application/json", "[]");
        }
    });
    
    // 2. Connect to specific device
    server->on("/api/connect", HTTP_GET, [](AsyncWebServerRequest* request) {
        Serial.println("[Web] API: /api/connect called");
        
        if (request->hasParam("id")) {
            int deviceId = request->getParam("id")->value().toInt();
            Serial.printf("[Web] Connecting to device ID: %d\n", deviceId);
            
            if (bluetoothConnectCallback) {
                bluetoothConnectCallback(deviceId);
                request->send(200, "application/json", "{\"status\":\"connecting\"}");
            } else {
                request->send(500, "application/json", "{\"error\":\"Bluetooth connect not available\"}");
            }
        } else {
            request->send(400, "application/json", "{\"error\":\"Missing device ID\"}");
        }
    });
    
    // 3. Get system status
    server->on("/api/status", HTTP_GET, [](AsyncWebServerRequest* request) {
        DynamicJsonDocument doc(256);
        doc["wifi_ip"] = WiFi.softAPIP().toString();
        doc["wifi_ssid"] = WiFi.softAPSSID();
        doc["connected"] = false;
        doc["device_count"] = 0;
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });
    
    // 4. Get system info
    server->on("/api/info", HTTP_GET, [](AsyncWebServerRequest* request) {
        DynamicJsonDocument doc(512);
        doc["chip_model"] = ESP.getChipModel();
        doc["free_heap"] = ESP.getFreeHeap();
        doc["flash_size"] = ESP.getFlashChipSize();
        doc["sdk_version"] = ESP.getSdkVersion();
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });
    
    // 5. 404 Error handler
    server->onNotFound([](AsyncWebServerRequest* request) {
        Serial.printf("[Web] 404: %s\n", request->url().c_str());
        
        // Try to serve 404.html if exists
        if (LittleFS.exists("/404.html")) {
            request->send(LittleFS, "/404.html", "text/html");
        } else {
            String message = "File Not Found\n\n";
            message += "URI: " + request->url() + "\n";
            message += "Method: " + String((request->method() == HTTP_GET) ? "GET" : "POST") + "\n";
            message += "Go to: <a href=\"/\">Home</a>\n";
            request->send(404, "text/plain", message);
        }
    });
    
    // ========================
    // Start Server
    // ========================
    server->begin();
    Serial.println("[Web] HTTP server started");
    Serial.printf("[Web] Open browser to: http://%s\n", WiFi.softAPIP().toString().c_str());
}