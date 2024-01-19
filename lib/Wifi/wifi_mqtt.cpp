#include <wifi_mqtt.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "storage.h"
#include "data_parser.h"
#include "ardu_cam.h"
#include "lock_control.h"
#include "mode.h"


dev_config_t *wdevice_config;
event_flags_t *wp_event_flags;

WiFiClient espClient;
PubSubClient wifi_client(espClient);
char wifimqttbrokerEndpoint[BROKER_ADDR_MAX_LEN] = {0};


int intToAscii(int number) {
   return '0' + number;
}
void mqtt_callback_wifi(char* topic, byte* message, unsigned int length)
{
    mqtt_callback_handle(topic,message,length);

}

static void reconnect() {
//    while (!wifi_client.connected()) {
    if (!wifi_client.connected()) {
        Serial.print("Attempting MQTT connection...");    
        Serial.print("Brokder IP/ADDR: ");
        Serial.println(wdevice_config->BROKER_ADDRESS);
        Serial.print("Broker port: ");
        Serial.println((uint16_t)atoi(wdevice_config->BROKER_PORT));
        if (wifi_client.connect("W_Client", wdevice_config->BROKER_USERNAME, DEFAULT_BROKER_PASSWORD)) {
            Serial.println("connected");
            memset(wifimqttbrokerEndpoint, 0x00, sizeof(wifimqttbrokerEndpoint));
            // Construct the topic
            snprintf(wifimqttbrokerEndpoint, SIZE_OF_TOPIC, "%s%c%s%c%s","Box",'/',wdevice_config->SERIAL_NUM,'/',"cmd");
            Serial.println(wifimqttbrokerEndpoint);
            wifi_client.subscribe(wifimqttbrokerEndpoint);
        } else {
            Serial.print("failed, rc=");   
            Serial.print(wifi_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void wifi_setup(dev_config_t *dev_config_param,event_flags_t *process_events)
{
    wdevice_config   = dev_config_param;
    wp_event_flags    = process_events;

    dev_config_t dev_config_wifi_cred; //to read wifi cred from memory

    int wifi_connect_retries = 0;
    Serial.print("Connecting to..");
    Serial.println(wdevice_config->WIFI_SSID);
    if(WiFi.status() == WL_CONNECTED)
    {
        WiFi.disconnect();
        delay(100);
    }
    
    WiFi.begin(wdevice_config->WIFI_SSID, wdevice_config->WIFI_PASSWORD);
    while(WiFi.status() != WL_CONNECTED) 
    {
        if(wifi_connect_retries < MAX_WIFI_RECONNECT_RETRIES)
        {
            wifi_connect_retries++;
        }
        else if(wifi_connect_retries >= MAX_WIFI_RECONNECT_RETRIES && wp_event_flags->update_wifi_cred)
        {
            //fetch from memory
            Serial.println("Read wifi from nvs");
            if(is_nvs_stored_config_updated(&dev_config_wifi_cred))
            {
                strncpy(wdevice_config->WIFI_SSID, dev_config_wifi_cred.WIFI_SSID, WIFI_SSID_MAX_LEN);
                strncpy(wdevice_config->WIFI_PASSWORD, dev_config_wifi_cred.WIFI_PASSWORD, WIFI_PASSWORD_MAX_LEN);
            }
            else
            {
                //use default
                strncpy(wdevice_config->WIFI_SSID, DEFAULT_WIFI_SSID, WIFI_SSID_MAX_LEN);
                strncpy(wdevice_config->WIFI_PASSWORD, DEFAULT_WIFI_PASSWORD, WIFI_PASSWORD_MAX_LEN);
            }
            Serial.print("Connecting to..");
            Serial.println(wdevice_config->WIFI_SSID);
            WiFi.begin(wdevice_config->WIFI_SSID, wdevice_config->WIFI_PASSWORD);
            wp_event_flags->update_wifi_cred = false;

        }
        delay(15000);
        Serial.print(".");  
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP Address");
    Serial.println(WiFi.localIP());
    if(wp_event_flags->update_wifi_cred)
    {
        //save updated config if available
        write_updated_config_to_nvs(wdevice_config,wp_event_flags);
    }

    Serial.print("Brokder IP/ADDR: ");
    Serial.println(wdevice_config->BROKER_ADDRESS);
    Serial.print("Broker port: ");
    Serial.println((uint16_t)atoi(wdevice_config->BROKER_PORT));
    wifi_client.setServer(wdevice_config->BROKER_ADDRESS, (uint16_t)atoi(wdevice_config->BROKER_PORT));
    wifi_client.setCallback(mqtt_callback_wifi);

    if(!wifi_client.connected())
    {
        reconnect();
    }
}
void wifi_disconnect()
{
    if(WiFi.status() == WL_CONNECTED)
    {
        WiFi.disconnect();
    }
}

void publish_ack_wifi(char* topic)
{
    wifi_client.publish(topic, "ACK");
}

void publish_lock_status_wifi(char* topic,bool status)
{
    if(status == true)
    {
        wifi_client.publish(topic, "Locked");
    }
    else
        wifi_client.publish(topic, "Not Locked");
}
void publish_image_wifi(char* topic)
{
    char image_chunk[50] = {0};
    String payload;
    char padding[4];
    uint8_t * image = read_image_from_nvs();
    if(image != NULL)
    {
        for(int i = 0; i < IMG_BUFFER_SIZE; i+=MQTT_MAX_BYTES)
        {
            
            payload +=  wdevice_config->SERIAL_NUM;         // 9 bytes
            payload += "C";                                // 1 byte
            payload += MQTT_MAX_BYTES;                     // 1 byte
            for(int j = i; j < (MQTT_MAX_BYTES+i); j++)    //payload : 107bytes
            {
                payload += image[j];
            }
            Serial.println(payload);
            wifi_client.publish(topic,payload.c_str());          //total payload= 1+9+1+1+107+1 = 120
            payload.clear();
        }
        //send remaining data
        payload +=  wdevice_config->SERIAL_NUM;
        payload += "N";
        sprintf(padding,"%03d",(IMG_BUFFER_SIZE - (MQTT_MAX_BYTES * (IMG_BUFFER_SIZE/MQTT_MAX_BYTES))));
        payload += padding;

        for(int k = (MQTT_MAX_BYTES * (IMG_BUFFER_SIZE/MQTT_MAX_BYTES)) ; k < IMG_BUFFER_SIZE; k++)
        {
            payload += image[k];
        }
        Serial.println(payload);
        wifi_client.publish(topic,payload.c_str());
        payload.clear(); 
    }
    else
    {
        wifi_client.publish(topic,"NACK");
    }
}

void refersh_wifi_mqtt_session(event_flags_t *process_events)
{
    if(process_events->device_serial_number_updated)
    {
        Serial.println("Let's subscribe to new serial number topic");
        wifi_client.disconnect();
        process_events->device_serial_number_updated = false;
    }

    if(!wifi_client.connected())
    {
        reconnect();
    } 
    wifi_client.loop();
}





