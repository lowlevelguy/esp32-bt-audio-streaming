#include "webserver.h"
#include "bluetooth.h"
#include <WiFi.h>

extern String* btDevices;
extern int btDeviceCount;

// Function prototypes from bluetooth module
extern void scanBluetoothDevices();
extern void connectToBluetoothDevice(int id);

void handleRoot() {
    // Scan for devices
    scanBluetoothDevices();
    
    WebServer* server = (WebServer*)&server; // Access global server
    
    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body { font-family: Arial; padding: 20px; }
.device { padding: 15px; margin: 10px; background: #007bff; color: white; 
          text-decoration: none; display: block; border-radius: 5px; }
</style>
</head>
<body>
<h1>Select Bluetooth Speaker</h1>
)rawliteral";
    
    server->sendContent(html);
    
    if (btDeviceCount == 0) {
        server->sendContent("<p>Scanning... refresh in 3 seconds</p>");
        server->sendContent("<script>setTimeout(()=>location.reload(),3000)</script>");
    } else {
        for (int i = 0; i < btDeviceCount; i++) {
            String link = "<a class='device' href='/connect?id=" + String(i) + "'>" + 
                         btDevices[i] + "</a>";
            server->sendContent(link);
        }
    }
    
    server->sendContent("</body></html>");
}

void handleConnect() {
    WebServer* server = (WebServer*)&server;
    
    String message;
    if (server->hasArg("id")) {
        int id = server->arg("id").toInt();
        if (id >= 0 && id < btDeviceCount) {
            connectToBluetoothDevice(id);
            message = "<h1>Connected to " + btDevices[id] + "!</h1>";
            message += "<p>System will reboot in 5 seconds...</p>";
            message += "<script>setTimeout(()=>{},5000)</script>";
        } else {
            message = "<h1>Invalid device ID</h1>";
        }
    } else {
        message = "<h1>Missing device ID</h1>";
    }
    
    message += "<p><a href='/'>Back</a></p>";
    server->send(200, "text/html", message);
}

void setupWebServer(WebServer* server) {
    server->on("/", handleRoot);
    server->on("/connect", handleConnect);
    
    server->begin();
    Serial.println("Lightweight web server started");
}