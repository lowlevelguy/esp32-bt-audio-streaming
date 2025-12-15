#include "wifi_ap.h"
#include "bluetooth.h"
#include "webserver.h"
#include "sd.h"

extern BluetoothA2DPSource a2dp;
extern std::vector<std::string> bt_devices;
extern int current_device_id;
extern bool streaming;

// Global variable to indicate the current state of the program
enum {
	SETUP_MODE,
	STREAMING_MODE
} mode;

void setup() {
	Serial.begin(9600);
	bt_init();
	bt_scan();

}

void loop() {
	Serial.println("Display Decteced devices");
	for (auto e : bt_devices){
			Serial.println(e);
	}
	delay(2000);
}
