#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include "./../config.h"

void wifi_setup(dev_config_t *dev_config_param,event_flags_t *process_events);
void refersh_wifi_mqtt_session(event_flags_t *process_events);
void wifi_disconnect(void);

void publish_ack_wifi(char* topic);
void publish_lock_status_wifi(char* topic,bool status);
void publish_image_wifi(char* topic);

#endif