#include "bluetooth.h"
#include "BluetoothA2DPSource.h"

/* ==============================
   VARIABLES GLOBALES
   ============================== */

BluetoothA2DPSource a2dp;
std::vector<std::string> bt_devices;
int current_device_id = -1;
bool streaming = false;

/* ==============================
   PROTOTYPE CALLBACK AUDIO
   ============================== */
extern int32_t sd_read_audio(uint8_t *data, int32_t len);

/* ==============================
   CALLBACK SCAN BLUETOOTH
   ============================== */
void bt_scan_callback(const char *name, esp_bd_addr_t addr, int rssi) {
    if (name != nullptr && strlen(name) > 0) {
        bt_devices.push_back(std::string(name));
    }
}

/* ==============================
   INITIALISATION BLUETOOTH
   ============================== */
void bt_init() {
    a2dp.set_scan_result_callback(bt_scan_callback);
}

/* ==============================
   MODE SETUP
   ============================== */

/* Scan des appareils Bluetooth */
void bt_scan() {
    bt_devices.clear();
    a2dp.scan();
}

/* Retourne la liste des devices détectés */
std::vector<std::string> bt_get_devices() {
    return bt_devices;
}

/* Connexion à un appareil Bluetooth */
bool bt_connect(int id) {
    if (id < 0 || id >= bt_devices.size())
        return false;

    current_device_id = id;
    return a2dp.connect_to(bt_devices[id].c_str());
}

/* ==============================
   MODE STREAMING
   ============================== */

/* Callback audio A2DP */
int32_t bt_audio_data_callback(uint8_t *data, int32_t len) {
    return sd_read_audio(data, len);
}

/* Démarrer le streaming audio */
bool bt_stream() {
    if (current_device_id < 0)
        return false;

    a2dp.set_stream_reader(bt_audio_data_callback);
    a2dp.start(bt_devices[current_device_id].c_str());
    streaming = true;
    return true;
}

/* Arrêter le streaming */
void bt_stop_stream() {
    a2dp.stop();
    streaming = false;
}

/* ==============================
   STATUS
   ============================== */

bool bt_is_connected() {
    return a2dp.is_connected();
}

bool bt_is_streaming() {
    return streaming;
}
