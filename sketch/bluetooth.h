#ifndef ESP32BTAUDIO_BLUETOOTH
#define ESP32BTAUDIO_BLUETOOTH

#include <stdint.h>
#include <BluetoothA2DPSource.h>
#include <esp_gap_bt_api.h>
#include <esp_bt_main.h>
#include <esp_bt.h>

#define MAX_DEV_NAME_LEN 32

typedef struct {
    char dev_ssid[MAX_DEV_NAME_LEN+1];
    esp_bd_addr_t dev_addr;
    int dev_rssi;
    uint32_t dev_cod;
} dev_info_t;


/*
 * Initializes A2DP, and sets up Bluetooth device discovery parameters (callback, scan mode).
 *
 * @param a2dp: reference to a BluetoothA2DPSource object
 */
void bt_init(BluetoothA2DPSource& a2dp);

/*
 * Enables Bluetooth device discovery mode for duration many milliseconds.
 *
 * @param duration: device discovery duration in milliseconds.
 */
void bt_discover(uint8_t duration = 10);

/*
 * Sets the target sink for A2DP.
 *
 * @param a2dp: reference to a BluetoothA2DPSource object
 * @param dev: sink device information
 */
void bt_connect(BluetoothA2DPSource& a2dp, dev_info_t dev);

/*
 * Sets the A2DP data callback to be used for fetching audio data.
 *
 * @param a2dp: reference to a BluetoothA2DPSource object
 * @param cb: function pointer to the target data callback
 */
void bt_stream(BluetoothA2DPSource& a2dp, int32_t (*cb)(uint8_t*,int32_t));

#endif
