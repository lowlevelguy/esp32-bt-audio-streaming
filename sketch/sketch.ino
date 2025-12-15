#include "wifi_ap.h"
#include "bluetooth.h"
#include "webserver.h"
#include "sd.h"


const size_t targets_row_count = 16, targets_row_sz = 33;
char targets[targets_row_count * targets_row_sz] = {0};
SdFat sd;

// Global variable to indicate the current state of the program
enum {
	SETUP_MODE,
	STREAMING_MODE
} mode;

void setup() {
	Serial.begin(9600);

	sd.begin(SS, SD_SCK_MHZ(4));
}

void loop() {
	size_t len;
	if ((len = parse_config()) > 0) {
		Serial.println("Config:");
		for (int i = 0; i < len; i++)
			Serial.println(&targets[i*targets_row_sz]);
	} else {
		Serial.println("Failed to read config file");
	}

	delay(2000);
}
