#ifndef ESP32BTAUDIO_BLUETOOTH
#define ESP32BTAUDIO_BLUETOOTH


#include <vector>
#include <string>
#include <stdint.h>

/* ========= SETUP MODE ========= */
void bt_init();
void bt_scan();
std::vector<std::string> bt_get_devices();
bool bt_connect(int id);

/* ========= STREAM MODE ========= */
bool bt_stream();
void bt_stop_stream();

/* ========= STATUS ========= */
bool bt_is_connected();
bool bt_is_streaming();



#endif
