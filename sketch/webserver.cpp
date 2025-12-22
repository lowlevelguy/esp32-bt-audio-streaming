#include "webserver.h"
#include "bluetooth.h"

#define MAX_BT_DEVS_COUNT 10

typedef enum {
	SETUP_MODE,
	STREAMING_MODE
} progmode_t;
extern progmode_t mode;
extern BluetoothA2DPSource a2dp;
extern std::vector<dev_info_t> bt_devs;

WebServer* server;

void handleRoot() {
    String html;

    if (mode == SETUP_MODE) {
        html = R"rawliteral(
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
<h1>Select Bluetooth Device to Connect to</h1>
)rawliteral";
    
        bt_discover(10);

        if (bt_devs.size() == 0) {
            html += "<p>No nearby Bluetooth device was detected. Please try again later</p>";
        } else {
            for (int i = 0; i < bt_devs.size(); i++) {
                String link = "<a class='device' href='/connect?id=" + String(i) + "'>";
                link.concat(bt_devs[i].dev_ssid);
                link += "</a>";
                html += link;
            }
        }
        html += "</body></html>";
    } else {
        html = "<h1>Already in streaming mode!</h1>";
    }

    server->send(200, "text/html", html);
}

void handleConnect() {
    String message;
    if (mode == SETUP_MODE) {
        if (server->hasArg("id")) {
            int id = server->arg("id").toInt();
            if (id >= 0 && id < bt_devs.size()) {
                bt_connect(a2dp, bt_devs[id]);
                delay(5000);
                if (a2dp.is_connected()) {
                    message = "<h1>Connected to ";
                    message.concat(bt_devs[id].dev_ssid);
                    message += "!</h1>";
                    mode = STREAMING_MODE;
                } else {
                    message = "<h1>Failed to connect!</h1>";
                }
            } else {
                message = "<h1>Invalid device ID</h1>";
            }
        } else {
            message = "<h1>Missing device ID</h1>";
        }
    } else {
        message = "<h1>Already in streaming mode!</h1>";
    }
    
    server->send(200, "text/html", message);
}

void setupWebServer(WebServer* srv) {
    server = srv;
    server->on("/", handleRoot);
    server->on("/connect", handleConnect);
    
    server->begin();
    Serial.println("Lightweight web server started");
}