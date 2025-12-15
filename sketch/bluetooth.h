#ifndef ESP32BTAUDIO_BLUETOOTH
#define ESP32BTAUDIO_BLUETOOTH


#include <vector>
#include <string>
#include <stdint.h>

// Initialize bluetooth module
void bt_init();

// Scan for bluetooth devices
// Returns: pointer to scan results string (format: "Name|Address\n" per device)
char* bt_scan();

// Connect to a bluetooth device (does NOT start streaming)
// Parameters:
//   bt_dev: Bluetooth device address (e.g., "AA:BB:CC:DD:EE:FF") or name
// Returns: 0 on success, -1 on failure
int bt_connect(const char* bt_dev);

// Set up audio streaming with a callback function
// This is when streaming actually starts
// Parameters:
//   callback: Function that fills audio buffer
//             Parameters: (uint8_t* data, int32_t len)
//             Returns: number of bytes written
void bt_stream(int32_t (*callback)(uint8_t*, int32_t));

// Check if currently connected
bool bt_is_connected();

// Disconnect from current device
void bt_disconnect();

// Get connection state
const char* bt_get_state();



#endif
