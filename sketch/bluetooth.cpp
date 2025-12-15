#include "bluetooth.h"
#include "BluetoothA2DPSource.h"
#include "BluetoothSerial.h"

// Global instances
static BluetoothA2DPSource a2dp_source;
static BluetoothSerial bt_serial;

// Audio callback storage
static int32_t (*audio_callback)(uint8_t*, int32_t) = nullptr;

// Scan results storage
static char scan_results[2048];
static bool scan_complete = false;

// Scan callback
static void bt_scan_callback(BTScanResults* results) {
    scan_results[0] = '\0';
    int offset = 0;
    
    for (int i = 0; i < results->getCount(); i++) {
        BTAdvertisedDevice* device = results->getDevice(i);
        if (device) {
            const char* name = device->getName().c_str();
            const char* addr = device->getAddress().toString().c_str();
            
            // Format: "Name|Address\n"
            offset += snprintf(scan_results + offset, 
                             sizeof(scan_results) - offset,
                             "%s|%s\n", 
                             name[0] ? name : "Unknown", 
                             addr);
            
            if (offset >= sizeof(scan_results) - 100) break;
        }
    }
    scan_complete = true;
}

// A2DP audio data callback wrapper
static int32_t a2dp_data_callback(uint8_t* data, int32_t len) {
    if (audio_callback != nullptr) {
        return audio_callback(data, len);
    }
    // Return silence if no callback
    memset(data, 0, len);
    return len;
}

// Initialize bluetooth module
void bt_init() {
    scan_results[0] = '\0';
    scan_complete = false;
}

// Scan for bluetooth devices
// Returns: pointer to scan results string (format: "Name|Address\n" per device)
char* bt_scan() {
    scan_results[0] = '\0';
    scan_complete = false;
    
    if (!bt_serial.begin("ESP32_Scanner", true)) {
        strcpy(scan_results, "ERROR: Failed to start Bluetooth\n");
        return scan_results;
    }
    
    // Discover devices (scan for 5 seconds)
    BTScanResults* results = bt_serial.discover(5000);
    
    if (results) {
        bt_scan_callback(results);
    } else {
        strcpy(scan_results, "ERROR: Scan failed\n");
    }
    
    bt_serial.end();
    return scan_results;
}

// Connect to a bluetooth device (does NOT start streaming)
// Parameters:
//   bt_dev: Bluetooth device address (e.g., "AA:BB:CC:DD:EE:FF") or name
// Returns: 0 on success, -1 on failure
int bt_connect(const char* bt_dev) {
    if (bt_dev == nullptr || bt_dev[0] == '\0') {
        return -1;
    }
    
    // Start A2DP source but don't set up streaming yet
    a2dp_source.start((char*)bt_dev);
    
    // Wait a bit for connection to establish
    delay(1000);
    
    if (a2dp_source.is_connected()) {
        return 0;
    }
    
    return -1;
}

// Set up audio streaming with a callback function
// This is when streaming actually starts
// Parameters:
//   callback: Function that fills audio buffer
//             Parameters: (uint8_t* data, int32_t len)
//             Returns: number of bytes written
void bt_stream(int32_t (*callback)(uint8_t*, int32_t)) {
    if (callback == nullptr) {
        return;
    }
    
    audio_callback = callback;
    // This call starts the actual audio streaming
    a2dp_source.set_stream_reader(a2dp_data_callback, false);
}

// Check if currently connected
bool bt_is_connected() {
    return a2dp_source.is_connected();
}

// Disconnect from current device
void bt_disconnect() {
    a2dp_source.disconnect();
    audio_callback = nullptr;
}

// Get connection state
const char* bt_get_state() {
    if (a2dp_source.is_connected()) {
        return "CONNECTED";
    }
    return "DISCONNECTED";
}