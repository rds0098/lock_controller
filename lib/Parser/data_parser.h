
#ifndef BLE_HELPER_H
#define BLE_HELPER_H

#include <stdio.h>
#include "./../config.h"



bool parseDevSettingsData(dev_config_t *dev_config,  uint8_t *rcv_data,event_flags_t *event_flag);
bool parseDevConfigData(dev_config_t *default_dev_config,  uint8_t *data,event_flags_t *event_flag);

// void handleWiFiConfig(dev_config_t *default_dev_config, uint8_t *data);
// void handleGPRSConfig(dev_config_t *default_dev_config, uint8_t *data);
// void handleBrokerConfig(dev_config_t *default_dev_config, uint8_t *data);
// void handleModeSelect(dev_config_t *default_dev_config, uint8_t* data);
// void handleKeypadMode(dev_config_t *default_dev_config, uint8_t* data);
// void handleReboot(void);
// void handleBoxUnlock(void);
// void handleImageCapture(void);
// void handleImagesRetrieve(dev_config_t *default_dev_config);
// void handlePasscodesUpdate(uint8_t* data);
// void handlePasscodeUnlock(uint8_t* data);
// void handleAESConfig(dev_config_t *default_dev_config, uint8_t* data);
// void handleFactoryReset(dev_config_t *default_dev_config);
// void handleSerialNumConfig(dev_config_t *default_dev_config, uint8_t* data);
// bool is_serial_num_correct(dev_config_t *default_dev_config, uint8_t* data);

#endif