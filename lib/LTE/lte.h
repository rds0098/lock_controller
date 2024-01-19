#ifndef LTE_H
#define LTE_H

#include "./../config.h"

//TTGO T-Call pins
#define MODEM_RST        5
#define MODEM_PWKEY      4
#define MODEM_POWER_ON  23
#define MODEM_TX        27
#define MODEM_RX        26
// #define I2C_SDA         21
// #define I2C_SCL         22

#define SerialAT  Serial1

#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER    1024

void modem_deinit(void);

bool gprs_mqtt_connect(void);
void gprs_mqtt_disconnect(void);
void modem_setup(dev_config_t *dev_config_param,event_flags_t *process_events);
void refresh_gprs_mqtt_session(dev_config_t *dev_config_param,event_flags_t *process_events);

void publish_ack_gprs(char* topic);
void publish_lock_status_gprs(char* topic,bool status);
void publish_image_gprs(char* topic);

#endif