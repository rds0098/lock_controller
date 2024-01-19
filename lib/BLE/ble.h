#ifndef BLE_H
#define BLE_H

#include "./../config.h"

#define USER_SERVICE_UUID                   "4fafc201-1fb5-459e-8fcc-c6d9c331915b" 
#define USER_CHARACTERISTIC_UUID            "beb5483e-36e1-4688-b7f5-ea07461a26b6"   

#define CONFIG_SERVICE_UUID                "beb5483e-36e1-4688-b7f5-ea07461a26b7" 
#define CONFIG_CHARACTERISTIC_UUID         "beb5483e-36e1-4688-b7f5-ea07461a26b8"   

#define APP_PAIRING_PASS_CODE               123456

#define BLE_MAX_DATA_BYTES 600

void ble_init(dev_config_t *device_setting,event_flags_t *p_events);
void start_advertsing(void);
void stop_advertising(void);

#endif