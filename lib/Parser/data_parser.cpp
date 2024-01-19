#include <Arduino.h>
#include "data_parser.h"
#include "lock_control.h"
#include "ardu_cam.h"
#include "storage.h"
#include "./../config.h"

extern char pass_str[10][7];

bool isValidChar(char c) 
{
    return (c >= '0' && c <= '9') || c == ',';
}

void handleWiFiConfig(dev_config_t *default_dev_config, uint8_t *data,event_flags_t *process_event)
{
    int length_ssid = 0;
    int length_password = 0;
    char len[3] = {0};

    strncpy(len,(char*)data+10,2);
    length_ssid = atoi(len);

    strncpy(len,(char*)data+12,2);
    length_password = atoi(len);
    Serial.print("ssid length: ");
    Serial.print(length_ssid);
    Serial.print("password length: ");
    Serial.print(length_password);

    if(length_ssid <= WIFI_SSID_MAX_LEN && length_password <= WIFI_PASSWORD_MAX_LEN)
    {
        //update wifi ssid
        memset(default_dev_config->WIFI_SSID, 0x00, WIFI_SSID_MAX_LEN);
        strncpy(default_dev_config->WIFI_SSID,(char*)(data+14),length_ssid);

        //update wifi password
        memset(default_dev_config->WIFI_PASSWORD, 0x00, WIFI_PASSWORD_MAX_LEN);
        strncpy(default_dev_config->WIFI_PASSWORD,(char*)(data+(14+length_ssid)),length_password);

        Serial.print("SSID: ");
        Serial.println((char*)default_dev_config->WIFI_SSID);
        Serial.print("Password: ");
        Serial.println((char*)default_dev_config->WIFI_PASSWORD);
        process_event->update_wifi_cred = true;
       // process_event->update_structure_in_nvs = true;
    }
    else
        Serial.println("Please Enter correct lengths");
}

void handleGPRSConfig(dev_config_t *default_dev_config, uint8_t *data,event_flags_t *process_event)
{
    int apn_length;
    int gprs_user_length;
    int gprs_password_length;
    int sim_pin_length;
    char len[3];
    strncpy(len,(char*)data+10,2);
    apn_length = atoi(len);
    strncpy(len,(char*)data+12,2);
    gprs_user_length = atoi(len);
    strncpy(len,(char*)data+14,2);
    gprs_password_length = atoi(len);

    sim_pin_length = atoi((char*)data+16);
    if(apn_length <= APN_MAX_LEN && gprs_user_length <= GPRS_USERNAME_MAX_LEN && gprs_password_length <= GPRS_PASSWORD_MAX_LEN && sim_pin_length <= SIM_PIN_MAX_LEN)
    {
        strncpy(default_dev_config->APN,(char*)(data+17),apn_length);
        strncpy(default_dev_config->GPRS_USER,(char*)(data+(17+apn_length)),gprs_user_length);
        strncpy(default_dev_config->GPRS_PASSWORD,(char*)(data+(17+apn_length+gprs_user_length)),gprs_password_length);
        strncpy(default_dev_config->SIM_PIN,(char*)(data+(17+apn_length+gprs_user_length+gprs_password_length)),sim_pin_length);
        default_dev_config->SIM_PIN[4] = '\0';
        
        Serial.println(apn_length);
        Serial.println(gprs_user_length);
        Serial.println(gprs_password_length);
        Serial.println(sim_pin_length);

        Serial.print("APN: ");
        Serial.println((char*)default_dev_config->APN);
        Serial.print("GPRS USERNAME: ");
        Serial.println((char*)default_dev_config->GPRS_USER);
        Serial.print("GPRS PASSWORD: ");
        Serial.println((char*)default_dev_config->GPRS_PASSWORD);
        Serial.print("SIM PIN: ");
        Serial.println((char*)default_dev_config->SIM_PIN);
        process_event->update_structure_in_nvs = true;
        write_updated_config_to_nvs(default_dev_config,process_event);
    }
    else
        Serial.println("Please Enter Correct lengths");
}

void handleBrokerConfig(dev_config_t *default_dev_config, uint8_t *data,event_flags_t *process_event)
{
    int broker_addr_length = 0;
    int broker_username_length = 0;
    int broker_password_length = 0;
    int port_length = 0;

    char len[3];

    memset(len,0,sizeof(len));
    strncpy(len,(char*)data+10,2);
    broker_addr_length = atoi(len);
    memset(len,0,sizeof(len));
    strncpy(len,(char*)data+12,2);
    broker_username_length = atoi(len);
    memset(len,0,sizeof(len));
    strncpy(len,(char*)data+14,2);
    broker_password_length = atoi(len);
    memset(len,0,sizeof(len));
    port_length = atoi((char*)data+16);

    Serial.println(broker_addr_length);
    Serial.println(broker_username_length);
    Serial.println(broker_password_length);
    Serial.println(port_length);

    if(broker_addr_length <= BROKER_ADDR_MAX_LEN && broker_username_length <= BROKER_USERNAME_MAX_LEN && broker_password_length <= BROKER_PASSWORD_MAX_LEN && port_length <= PORT_MAX_LEN)
    {
        strncpy(default_dev_config->BROKER_ADDRESS,(char*)(data+17),broker_addr_length);
        strncpy(default_dev_config->BROKER_USERNAME,(char*)(data+(17+broker_addr_length)),broker_username_length);
        strncpy(default_dev_config->BROKER_PASSWORD,(char*)(data+(17+broker_addr_length+broker_username_length)),broker_password_length);
        strncpy(default_dev_config->BROKER_PORT,(char*)(data+(17+broker_addr_length+broker_username_length+broker_password_length)),port_length);

        Serial.print("BROKER ADDRESS: ");
        Serial.println((char*)default_dev_config->BROKER_ADDRESS);
        Serial.print("BROKER USERNAME: ");
        Serial.println((char*)default_dev_config->BROKER_USERNAME);
        Serial.print("BROKER PASSWORD: ");
        Serial.println((char*)default_dev_config->BROKER_PASSWORD);
        Serial.print("PORT: ");
        Serial.println((char*)default_dev_config->BROKER_PORT);
        process_event->update_structure_in_nvs = true;
        write_updated_config_to_nvs(default_dev_config,process_event);
    }
    else
        Serial.println("Please Enter Correct lengths");
}

void handleModeSelect(dev_config_t *default_dev_config, uint8_t* data,event_flags_t *process_event)
{
    switch (data[10])
    {
        case 'W':
            Serial.println("MODE WIFI");
            default_dev_config->mode = MODE_WIFI;
            process_event->update_structure_in_nvs = true;
            write_updated_config_to_nvs(default_dev_config,process_event);
            break;
        case 'G':
            Serial.println("MODE LTE");
            default_dev_config->mode = MODE_LTE;
            process_event->update_structure_in_nvs = true;
            write_updated_config_to_nvs(default_dev_config,process_event);
            break;
        case 'B':
            Serial.println("MODE BLE");
            default_dev_config->mode = MODE_BLE;
            process_event->update_structure_in_nvs = true;
            write_updated_config_to_nvs(default_dev_config,process_event);
            break;
        
        default:
            break;
    }
             
}

void handleKeypadMode(dev_config_t *default_dev_config, uint8_t* data,event_flags_t *process_event)
{
    switch (data[10])
    {
        case '0':
            Serial.println("Disable Keypad");
            default_dev_config->keypad_enable = false;
            process_event->update_structure_in_nvs = true;
            write_updated_config_to_nvs(default_dev_config,process_event);
            break;
        case '1':
            Serial.println("Enable Keypad");
            default_dev_config->keypad_enable = true;
            process_event->update_structure_in_nvs = true;
            write_updated_config_to_nvs(default_dev_config,process_event);
            break;
        default:
            break;
    }
}

void handleReboot()
{
    Serial.println("Rebooting...");
    ESP.restart();
}

void handleBoxUnlock()
{
    Serial.println("Unlock Box");
    unlock_controller();
}

void handleImageCapture()
{
    Serial.println("Capture image & save");
    #ifndef TEST_WITHOUT_CAMERA
    Capture_Image(); 
    save_image_to_nvs();
    #else
    save_dummy_image_to_nvs();
    #endif 
}

void handleImagesRetrieve(dev_config_t *default_dev_config)
{
    default_dev_config->retrieve_image = true;
}

void handlePasscodesUpdate(uint8_t* mqtt_cmd)
{
    char *token;
    int idx = 0;
    bool pass_len_flag = true;
    //char data[SERIAL_NUM_LEN + 6*10 + 9 + 1] = {0};
    char data[SERIAL_NUM_LEN + 6*10 + 9 + 1 + 1] = {0};
 //   strncpy(data, (const char *)mqtt_cmd, sizeof(data));

    memset(pass_str,0,sizeof(pass_str));
    // token = strtok((char*)data+10,",");
    token = strtok((char*)mqtt_cmd+10,",");
    while(token != NULL)
    {
        if(strlen(token) != 6) 
        {
            int i;
            for(i = 0; token[i] != '\0'; i++) {
                if(!isValidChar(token[i])) {
                    break;
                }
            }
            token[i] = '\0';
            pass_len_flag = false;
        }

      //  Serial.println(token);
        strncpy(pass_str[idx],token, sizeof(pass_str[idx]));
        idx++;
        token = strtok(NULL, ",");
        
    }

    if((idx > MAX_NUM_PASS) || (idx < MAX_NUM_PASS))
    {
        Serial.println("Can not update passwords");
        Serial.println("Please Enter 10 passwords, 6 characters each");
    }
    else
    {
        Serial.println("Print new passwords");

        for(int i = 0; i< MAX_NUM_PASS; i++)
        {
            Serial.println(pass_str[i]);
        }
        write_passwords_to_nvs();
    }
    
}

void handlePasscodeUnlock(uint8_t* data)
{
    char received_password[10] = {0};
   // Serial.println("Inside handle function");
    // Serial.println((char*)(data+10));
   // strcpy(received_password,(char*)(data+10));
    strncpy(received_password,(char*)(data+10),PASS_NUM_LENGTH-1);
    Serial.println("Extracted password is:");
    Serial.println((char*)received_password);
    // Serial.println("password length is:");
    // Serial.println(strlen(received_password));
    if(strlen(received_password) != (PASS_NUM_LENGTH-1))
    {
        Serial.println("Please enter 6-digit password");
    }
    else
    {
        if(is_rcv_password_correct((char*)received_password))
        {
            Serial.println("Unlocked");
            unlock_controller();
            #ifndef TEST_WITHOUT_CAMERA
            Capture_Image(); 
            save_image_to_nvs();
            #endif
        }
        else
        {
            Serial.println("Incorrect password received");
        }
    }
}

void handleReadLockStatus(dev_config_t *default_dev_config)
{
    default_dev_config->read_lock_status = true;
}
void handleAESConfig(dev_config_t *default_dev_config, uint8_t* data,event_flags_t *process_event)
{

    strncpy(default_dev_config->AES,(char*)(data+10),AES_KEY_LEN);
    default_dev_config->AES[32] = '\0';
    Serial.println("UPDATED AES");
    Serial.println((char*)default_dev_config->AES);
    process_event->update_structure_in_nvs = true;
    write_updated_config_to_nvs(default_dev_config,process_event);
    
}

//void handleFactoryReset(dev_config_t *default_dev_config)
void handleFactoryReset(event_flags_t *e_flag)
{
    Serial.println("Input received for factory reset");
    e_flag->factory_reset = true;
}

void handleSerialNumConfig(dev_config_t *default_dev_config, uint8_t* data,event_flags_t *process_event)
{
    if(!strcmp(default_dev_config->SERIAL_NUM,(char*)(data+1)))
    {
        //do nothing
        Serial.println("Duplicated Serial number received");
    }
    else
    {
        strncpy(default_dev_config->SERIAL_NUM,(char*)(data+1),SERIAL_NUM_LEN);
       // strcat(default_dev_config->SERIAL_NUM, "\0");
       // serial_num_updated = true;
        Serial.print("Updated Serial Number: ");
        Serial.println((char*)default_dev_config->SERIAL_NUM);
        process_event->device_serial_number_updated = true;
        process_event->update_structure_in_nvs = true;
        write_updated_config_to_nvs(default_dev_config,process_event);
    }

}

bool is_serial_num_correct(dev_config_t *default_dev_config, uint8_t* data)
{
    char serial_num[SERIAL_NUM_LEN] = {0};
    strncpy((char*)serial_num, (char*)data, SERIAL_NUM_LEN);

    if(!strncmp((char*)default_dev_config->SERIAL_NUM, (char*)serial_num, SERIAL_NUM_LEN))
    {
        return true;
    }
    else
    {
        return false;
    }


}

bool parseDevSettingsData(dev_config_t *dev_config,  uint8_t *rcv_data,event_flags_t *event_flag)
{
    if(is_serial_num_correct(dev_config, rcv_data))
    {
        Serial.println("Correct serial number received");
        switch(rcv_data[9])
        {
            case 'W':
            case 'w':
                handleWiFiConfig(dev_config, rcv_data,event_flag);
            break;
            case 'G':
            case 'g':
                handleGPRSConfig(dev_config, rcv_data,event_flag);
            break;
            case 'B':
            case 'b':
                handleBrokerConfig(dev_config, rcv_data,event_flag);
            break;
            case 'M':
            case 'm':
                handleModeSelect(dev_config, rcv_data,event_flag);
            break;
            case 'K':
            case 'k':
                handleKeypadMode(dev_config, rcv_data,event_flag);
            break;
            case 'R':
            case 'r':
                handleReboot();     
            break;
            case 'U':
            case 'u':
                handleBoxUnlock();
            break;
            case 'C':
            case 'c':
                handleImageCapture();
            break;
            case 'I':
            case 'i':
                handleImagesRetrieve(dev_config);
            break;
            case 'S':
            case 's':
                handlePasscodesUpdate(rcv_data);    
            break;
            case 'P':
            case 'p':
                handlePasscodeUnlock(rcv_data);
            break;
            case 'T':
            case 't':
                if((dev_config->mode == MODE_WIFI) || (dev_config->mode == MODE_LTE))
                {
                    handleReadLockStatus(dev_config);
                }
            break;
            default:
                return 0;
            break;
        }
    }
    else{
     //   Serial.println("Entered Serial num is incorrect");
        return false;
    }
    return true;
}


bool parseDevConfigData(dev_config_t *dev_config,  uint8_t *rcv_data,event_flags_t *event_flag)
{
    if(rcv_data[0] == 's' || rcv_data[0] == 'S')
    {
        handleSerialNumConfig(dev_config, rcv_data,event_flag);
        return true;
    }
    else
    {
        if(is_serial_num_correct(dev_config, rcv_data))
        {
            switch(rcv_data[9])
            {
                case 'A':
                case 'a':
                    handleAESConfig(dev_config, rcv_data,event_flag);
                break;
                case 'Q':
                case 'q':
                    handleFactoryReset(event_flag);
                default:
                break;
            }
        }
        else
        {
            Serial.println("Entered Serial num is incorrect");
            return false;
        }
    }

    return true;
}
