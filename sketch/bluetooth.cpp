#include "bluetooth.h"
#include <iostream>

int min(int a, int b) {
    return (a < b) ? a : b;
}

static bool discovery_done = false;
std::vector<dev_info_t> bt_devs;

bool get_name_from_eir(uint8_t *eir, char *bdname, uint8_t *bdname_len) {
    if (!eir) return false;
    
    uint8_t *rmt_bdname = NULL;
    uint8_t rmt_bdname_len = 0;

    rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME, &rmt_bdname_len);
    if (!rmt_bdname) {
        rmt_bdname = esp_bt_gap_resolve_eir_data(eir, ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME, &rmt_bdname_len);
    }

    if (rmt_bdname) {
        if (rmt_bdname_len > MAX_DEV_NAME_LEN) {
            rmt_bdname_len = MAX_DEV_NAME_LEN;
        }
        if (bdname) {
            memcpy(bdname, rmt_bdname, rmt_bdname_len);
            bdname[rmt_bdname_len] = '\0';
        }
        if (bdname_len) {
            *bdname_len = rmt_bdname_len;
        }
        return true;
    }
    return false;
}

void update_device_info(esp_bt_gap_cb_param_t *param) {
    dev_info_t device;
    memset(&device, 0, sizeof(dev_info_t));
    memcpy(device.dev_addr, param->disc_res.bda, ESP_BD_ADDR_LEN);
    
    esp_bt_gap_dev_prop_t *p;
    for (int i = 0; i < param->disc_res.num_prop; i++) {
        p = param->disc_res.prop + i;
        
        switch (p->type) {
            case ESP_BT_GAP_DEV_PROP_COD:
                device.dev_cod = *(uint32_t *)(p->val);
                break;
            case ESP_BT_GAP_DEV_PROP_RSSI:
                device.dev_rssi = *(int8_t *)(p->val);
                break;
            case ESP_BT_GAP_DEV_PROP_BDNAME: {
                uint8_t len = (p->len > MAX_DEV_NAME_LEN) ? MAX_DEV_NAME_LEN : (uint8_t)p->len;
                memcpy(device.dev_ssid, (uint8_t *)(p->val), len);
                device.dev_ssid[len] = '\0';
                break;
            }
            case ESP_BT_GAP_DEV_PROP_EIR: {
                if (strlen(device.dev_ssid) == 0) {
                    uint8_t name_len = 0;
                    get_name_from_eir((uint8_t *)(p->val), device.dev_ssid, &name_len);
                }
                break;
            }
        }
    }
    
    if (strlen(device.dev_ssid) == 0) {
        strcpy(device.dev_ssid, "[Unknown]");
    }
    
    for (auto dev : bt_devs) {
        if (memcmp(device.dev_addr, dev.dev_addr, sizeof(esp_bd_addr_t)) == 0)
            return;
    }

    std::cout << "Device Detected:\n\tSSID: " << device.dev_ssid << "\n\tRSSI: " << device.dev_rssi << "\n";
    bt_devs.push_back(device);
}

void gap_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    std::cout << "Device detected\n";
    switch (event) {
        case ESP_BT_GAP_DISC_RES_EVT:
            update_device_info(param);
            break;
        case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
            if (param->disc_st_chg.state == ESP_BT_GAP_DISCOVERY_STOPPED) {
                discovery_done = true;
            }
            break;
    }
}

void bt_init(BluetoothA2DPSource& a2dp) {
    a2dp.set_local_name("ESP32 Audio Source");
    a2dp.set_pin_code("9999", ESP_BT_PIN_TYPE_FIXED);
    a2dp.set_ssp_enabled(true);
    a2dp.set_discoverability(ESP_BT_GENERAL_DISCOVERABLE);
    a2dp.set_reset_ble(true);
    a2dp.set_volume(100);
    
    a2dp.start();
    delay(2000);
    
    esp_bt_gap_register_callback(gap_callback);
    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
}

void bt_discover(uint8_t duration) {
    bt_devs.clear();
    discovery_done = false;
    
    esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, duration, 0);
    
    unsigned long start = millis();
    unsigned long timeout = (duration * 1280UL) + 3000;
    
    while (!discovery_done && (millis() - start < timeout)) {
        delay(100);
    }
    
    if (!discovery_done) {
        esp_bt_gap_cancel_discovery();
    }
}

void bt_connect(BluetoothA2DPSource& a2dp, dev_info_t dev) {
    a2dp.start(dev.dev_ssid);
}

void bt_stream(BluetoothA2DPSource& a2dp, int32_t (*cb)(uint8_t*,int32_t)) {
    a2dp.set_data_callback(cb);
}