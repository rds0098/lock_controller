#ifndef MODE_H
#define MODE_H

#include "./../config.h"

void apply_system_mode(dev_config_t *dev_config,event_flags_t *process_event);
void mqtt_callback_handle(char* topic, byte* message, unsigned int length);

#endif