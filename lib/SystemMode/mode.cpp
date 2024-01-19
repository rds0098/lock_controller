#include <Arduino.h>
#include <mode.h>

#include "wifi_mqtt.h"
#include <ble.h>
#include <lte.h>
#include <lock_control.h>
#include <data_parser.h>
#include "ardu_cam.h"

uint8_t prev_mode = 0;

dev_config_t *d_config;
event_flags_t *p_evt_flags;
char brokerEndpoint[BROKER_ADDR_MAX_LEN] = {0};

static void disconnect_prev_mode(uint8_t prev_mode)
{
    switch(prev_mode)
    {
        case MODE_WIFI:
            wifi_disconnect();
        break;
        case MODE_LTE:
            gprs_mqtt_disconnect();
        break;
        default:
        break;
    }
}


void mqtt_callback_handle(char* topic, byte* message, unsigned int length)
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    
    String messageTemp;
    bool lock_status = false;

    Serial.print(".Message: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    memset(brokerEndpoint, 0x00, sizeof(brokerEndpoint));
    snprintf(brokerEndpoint, SIZE_OF_TOPIC, "%s%c%s%c%s","Box",'/',d_config->SERIAL_NUM,'/',"cmd");

    Serial.println("Constructed topic");
    Serial.println(brokerEndpoint);
    // check if receive any command against my device
    if( !strncmp( topic, brokerEndpoint, strlen(topic) ) )
    {
        // Check if its Device Setting command
        if( !parseDevSettingsData(d_config, (uint8_t *)message,p_evt_flags) )
        {
            Serial.println("HAndled DEvice Settings ");
            // check if its Device Configuration command
            if( !parseDevConfigData(d_config, (uint8_t *)message,p_evt_flags) )
            {
                return;
            }
        }
        Serial.println("Constructing the response topic");
        snprintf(brokerEndpoint, SIZE_OF_RESPONSE, "%s%c%s%c%s","Box",'/',d_config->SERIAL_NUM,'/',"response");
        Serial.println(brokerEndpoint);
        if(d_config->read_lock_status == true)
        {
            lock_status = is_device_locked();
            if(d_config->mode == MODE_WIFI)
            {
                publish_lock_status_wifi(brokerEndpoint,lock_status);
            }
            else if(d_config->mode == MODE_LTE)
            {
                publish_lock_status_gprs(brokerEndpoint,lock_status);
            }
            else
            {

            }
            d_config->read_lock_status = false;
        }
        else if(d_config->retrieve_image == true)
        {
            if(d_config->mode == MODE_WIFI)
            {
                publish_image_wifi(brokerEndpoint);
            }
            else if(d_config->mode == MODE_LTE)
            {
                publish_image_gprs(brokerEndpoint);
            }
            else
            {

            }
            d_config->retrieve_image = false;
        }
        else
        {
            //send ACK
            if(d_config->mode == MODE_WIFI)
            {
                publish_ack_wifi(brokerEndpoint);
            }
            else if(d_config->mode == MODE_LTE)
            {
                publish_ack_gprs(brokerEndpoint);
            }
            else
            {

            }
        }
    }
}
void apply_system_mode(dev_config_t *dev_config,event_flags_t *process_event)
{
    d_config = dev_config;
    p_evt_flags = process_event;

    switch(dev_config->mode)
    {
        case MODE_WIFI:
            if(prev_mode != dev_config->mode || process_event->update_wifi_cred || process_event->apply_factory_reset)
            {

                disconnect_prev_mode(prev_mode);
                wifi_setup(dev_config,process_event);
                prev_mode = dev_config->mode;
                //clearing events after processing
                if(process_event->update_wifi_cred)
                    process_event->update_wifi_cred = false;
                if(process_event->apply_factory_reset)
                    process_event->apply_factory_reset = false;
            }
            else
                refersh_wifi_mqtt_session(process_event);
        break;
        case MODE_LTE:
            if(prev_mode != dev_config->mode || process_event->apply_factory_reset)
            {
                disconnect_prev_mode(prev_mode);
                modem_setup(dev_config,process_event);
                prev_mode = dev_config->mode;
                if(process_event->apply_factory_reset)
                    process_event->apply_factory_reset = false;
            }
            else
                refresh_gprs_mqtt_session(dev_config,process_event);
        break;
        case MODE_BLE: //if user wants only BLE and turn off Wifi/LTE
            if(prev_mode != dev_config->mode)
            {
                disconnect_prev_mode(prev_mode);
            }
        break;
        default:
        break;
    }
}