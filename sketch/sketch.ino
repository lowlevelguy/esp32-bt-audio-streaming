#include "wifi_ap.h"
#include "bluetooth.h"
#include "webserver.h"
#include "sd.h"

// Global variable to indicate the current state of the program
enum {
	SETUP_MODE,
	STREAMING_MODE
} mode;

void setup() {
	Serial.begin(9600);

}

void loop() {

}
