#include <lte.h>
//#include <Wire.h>
#include <TinyGSMClient.h>
#include <PubSubClient.h>
#include <storage.h>
#include <mode.h>

TinyGsm modem(SerialAT);


TinyGsmClient mqtt_client(modem);
PubSubClient mqtt(mqtt_client);

char ltemqttbrokerEndpoint[BROKER_ADDR_MAX_LEN] = {0};
dev_config_t *device_config;
event_flags_t *p_event_flags;

void mqtt_callback_lte(char* topic, byte* payload, unsigned int len) 
{
    mqtt_callback_handle(topic,payload,len);
}

void publish_ack_gprs(char* topic)
{
    mqtt.publish(topic, "ACK");
}

void publish_lock_status_gprs(char* topic,bool status)
{
    if(status == true)
    {
        mqtt.publish(topic, "Locked");
    }
    else
        mqtt.publish(topic, "Not Locked");
}
void publish_image_gprs(char* topic)
{
    char image_chunk[50] = {0};
    String payload;
    char padding[4];
    uint8_t * image = read_image_from_nvs();

    for(int i = 0; i < IMG_BUFFER_SIZE; i+=MQTT_MAX_BYTES)
    {
        
        payload +=  device_config->SERIAL_NUM;         // 9 bytes
        payload += "C";                                // 1 byte
        payload += MQTT_MAX_BYTES;                     // 1 byte
        for(int j = i; j < (MQTT_MAX_BYTES+i); j++)    //payload : 107bytes
        {
            payload += image[j];
        }
        Serial.println(payload);
        mqtt.publish(topic,payload.c_str());          //total payload= 1+9+1+1+107+1 = 120
        payload.clear();
    }
    //send remaining data
    payload +=  device_config->SERIAL_NUM;
    payload += "N";
    sprintf(padding,"%03d",(IMG_BUFFER_SIZE - (MQTT_MAX_BYTES * (IMG_BUFFER_SIZE/MQTT_MAX_BYTES))));
    payload += padding;

    for(int k = (MQTT_MAX_BYTES * (IMG_BUFFER_SIZE/MQTT_MAX_BYTES)) ; k < IMG_BUFFER_SIZE; k++)
    {
        payload += image[k];
    }
    Serial.println(payload);
    mqtt.publish(topic,payload.c_str());
    payload.clear(); 
}

void modem_deinit(void)
{
    modem.gprsDisconnect();
    SerialAT.end();
}
void modem_setup(dev_config_t *dev_config_param,event_flags_t *process_events)
{
    device_config = dev_config_param;
    p_event_flags    = process_events;

     pinMode(MODEM_PWKEY,OUTPUT);
     pinMode(MODEM_RST,OUTPUT);
     pinMode(MODEM_POWER_ON,OUTPUT);
     digitalWrite(MODEM_PWKEY,LOW);
     digitalWrite(MODEM_RST,HIGH);
     digitalWrite(MODEM_POWER_ON,HIGH);


    SerialAT.begin(115200,SERIAL_8N1,MODEM_RX,MODEM_TX);
    delay(3000);

    Serial.println("Initializing modem...");

    if(!modem.init())
    {
      modem.restart();
      delay(2000);
      Serial.println("Restarting modem");
    }
    if(strlen(device_config->SIM_PIN) && modem.getSimStatus() != 3)
    {
        modem.simUnlock(device_config->SIM_PIN);
    }
    String modeminfo = modem.getModemInfo();
    Serial.print("Modem Info: ");
    Serial.println(modeminfo);

    Serial.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    delay(10000);
    return;
    }
    Serial.println(" success");

    if (modem.isNetworkConnected()) { Serial.println("Network connected"); }

    Serial.print(F("Connecting to "));
    Serial.print(device_config->APN);
    if (!modem.gprsConnect(device_config->APN, device_config->GPRS_USER, device_config->GPRS_PASSWORD)) {
    Serial.println(" fail");
    delay(10000);
    
    return;
  }

   Serial.println(" success");

  if(modem.isGprsConnected()) 
  {
    Serial.println("GPRS connected"); 
  }

    //mqtt setup
    mqtt.setServer(device_config->BROKER_ADDRESS, (uint16_t)atoi(device_config->BROKER_PORT));
    mqtt.setCallback(mqtt_callback_lte);

    gprs_mqtt_connect();
}

bool gprs_mqtt_connect(void)
{

    bool status = mqtt.connect("mqttx_005fa98a");
    if (status == false) {
      Serial.println(" fail");
      return false;
    }
    Serial.println(" success");
    // Construct the topic
    snprintf(ltemqttbrokerEndpoint, SIZE_OF_TOPIC, "%s%c%s%c%s","Box",'/',device_config->SERIAL_NUM,'/',"cmd");
    Serial.println(ltemqttbrokerEndpoint);
    mqtt.subscribe(ltemqttbrokerEndpoint);

    return mqtt.connected();
  
}
void refresh_gprs_mqtt_session(dev_config_t *dev_config_param,event_flags_t *process_events)
{
    if(!modem.isNetworkConnected())
    {
        Serial.println("Network disconnected");
        if(!modem.waitForNetwork(180000L,true))
        {
            Serial.println("fail");
            delay(10000);
        }
    }
    if(modem.isNetworkConnected())
    {
    //    Serial.println("Network reconnected");
    }
    if(!modem.isGprsConnected())
    {
        Serial.println("GPRS disconnected");
        Serial.println("Connecting to...");
        Serial.println(dev_config_param->APN);
        if(!modem.gprsConnect(dev_config_param->APN,dev_config_param->GPRS_USER,dev_config_param->GPRS_PASSWORD))
        {
            Serial.println("fail");
            delay(1000);
        }
    }
    if(modem.isGprsConnected())
    {
    //    Serial.println("GPRS Connected");
    }
    if(!mqtt.connected())
    {
        Serial.println("MQTT Not Connected");
        gprs_mqtt_connect();
        
    }
    if(process_events->device_serial_number_updated)
    {
        mqtt.disconnect();
        gprs_mqtt_connect();
        process_events->device_serial_number_updated = false;
    }
    mqtt.loop();
}

void gprs_mqtt_disconnect(void)
{
    modem.gprsDisconnect();
}


