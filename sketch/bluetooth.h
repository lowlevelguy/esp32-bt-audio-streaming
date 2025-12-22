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

void bt_init(BluetoothA2DPSource& a2dp);
void bt_discover(uint8_t duration = 10);
void bt_connect(BluetoothA2DPSource& a2dp, dev_info_t dev);
void bt_stream(BluetoothA2DPSource& a2dp, int32_t (*cb)(uint8_t*,int32_t));

#endif
