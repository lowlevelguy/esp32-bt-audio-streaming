#include "wifi_ap.h"
#include "bluetooth.h"
#include "webserver.h"
#include "sd.h"

extern const size_t targets_row_count = 16, targets_row_sz = 33;
char targets[targets_row_count * targets_row_sz] = {0};
SdFat sd;
BluetoothA2DPSource a2dp;

// Global variable to indicate the current state of the program
typedef enum {
	SETUP_MODE,
	STREAMING_MODE
} progmode_t;
progmode_t mode = SETUP_MODE;

WebServer ws(8080);

void setup() {
	Serial.begin(9600);
	sd.begin(SS, SD_SCK_MHZ(4));

	// Begin in SETUP_MODE
	mode = SETUP_MODE;

	// Start AP and WebServer;
	IPAddress ap_addr = wifi_init();
	Serial.print("ESP32 working as WiFi access point... IP Address: ");
	Serial.println(ap_addr);

	bt_init(a2dp);
	bt_stream(a2dp, get_sound_wrapper);
	setupWebServer(&ws);
}

void loop() {
	ws.handleClient();
}