#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

#define WIFI_SSID_MAX_LEN        32
#define WIFI_PASSWORD_MAX_LEN    64
#define APN_MAX_LEN              20
#define GPRS_USERNAME_MAX_LEN    10
#define GPRS_PASSWORD_MAX_LEN    10
#define SIM_PIN_MAX_LEN           4
#define BROKER_ADDR_MAX_LEN      30
#define BROKER_USERNAME_MAX_LEN  30
#define BROKER_PASSWORD_MAX_LEN  30
#define PORT_MAX_LEN              5
#define AES_KEY_LEN              32
#define SERIAL_NUM_LEN            9 

#define PASS_NUM_LENGTH           7     // length of a singal password
#define MAX_NUM_PASS             10    // total number of passwords

// ------------------- Default Settings ----------------
//#define DEFAULT_WIFI_SSID          "ZONG4G-63C9"
#define DEFAULT_WIFI_SSID          "Jaddoumesh"
//#define DEFAULT_WIFI_PASSWORD      "02945111"
#define DEFAULT_WIFI_PASSWORD      "0502070751@"
#define DEFAULT_APN                "jawalnet.com.sa"
#define DEFAULT_GPRS_USER          ""
#define DEFAULT_GPRS_PASSWORD      ""
#define DEFAULT_SIM_PIN            ""
#define DEFAULT_BROKER_ADDRESS     "broker.emqx.io"
#define DEFAULT_BROKER_USERNAME    ""
#define DEFAULT_BROKER_PASSWORD    ""
#define DEFAULT_BROKER_PORT        "1883"
#define DEFAULT_AES                ""
#define DEFAULT_SERIAL_NUM         "RDS000000"

/***********************MQTT TOPICS********************************/
/*publish topic

  Box/RDS000001/response        -To send ACK after receiving commands,send image & lock status

  Subscribe topic
  Box/RDS000001/cmd             -To recieve commnads from mqtt broker  

  **Note: RDS000001 is default device serial number which will be changed according to user configuration.
*/


#define MQTT_MAX_BYTES       109
#define MQTT_PAYLOAD_SIZE    120
#define MQTT_PAYLOAD_HEADER  (MQTT_PAYLOAD_SIZE - MQTT_MAX_BYTES)
//mqtt topics
#define SIZE_OF_TOPIC       SERIAL_NUM_LEN + 3 + 2 + 3 + 1  // Serial_num + pre-fix[Box] + separator [/] + cmd[postfix] + null terminaots
#define SIZE_OF_RESPONSE    SERIAL_NUM_LEN + 3 + 2 + 8 + 1

#define MAX_WIFI_RECONNECT_RETRIES   3
// #define MAX_BROKER_RECONNECT_RETRIES 5
// #define MAX_GPRS_RECONNECT_RETRIES   5
typedef struct
{
    //user
    char WIFI_SSID[WIFI_SSID_MAX_LEN];
    char WIFI_PASSWORD[WIFI_PASSWORD_MAX_LEN];
    char APN[APN_MAX_LEN];
    char GPRS_USER[GPRS_USERNAME_MAX_LEN];
    char GPRS_PASSWORD[GPRS_PASSWORD_MAX_LEN];
    char SIM_PIN[SIM_PIN_MAX_LEN + 1];
    char BROKER_ADDRESS[BROKER_ADDR_MAX_LEN];
    char BROKER_USERNAME[BROKER_USERNAME_MAX_LEN];
    char BROKER_PASSWORD[BROKER_PASSWORD_MAX_LEN];
    char BROKER_PORT[PORT_MAX_LEN];
    uint8_t mode;
    bool keypad_enable;
    bool capture_image;
    bool retrieve_image;
    bool update_passcode;
    bool passcode_unlock;
   // bool factory_reset;
    bool read_lock_status;
    bool config_updated_in_nvs;

    //config
    char AES[AES_KEY_LEN + 1];
    char SERIAL_NUM[SERIAL_NUM_LEN + 1];

}dev_config_t;

typedef struct
{
    bool update_wifi_cred               : 1;
    bool device_serial_number_updated   : 1;
    bool update_structure_in_nvs        : 1;
    bool factory_reset                  : 1;
    bool apply_factory_reset            : 1;

}event_flags_t;

typedef enum 
{
    MODE_WIFI = 1,
    MODE_LTE,
    MODE_BLE
}sys_mode_t;

#endif