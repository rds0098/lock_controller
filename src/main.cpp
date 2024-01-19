#include <Arduino.h>
#include <SPI.h>
#include <ble.h>
#include <key_pad.h>
#include <lock_control.h>
#include "ardu_cam.h"
#include "storage.h"
#include "./../lib/config.h"
#include <mode.h>

bool print_once = true;

dev_config_t dev_config_param;
event_flags_t process_event;

dev_config_t dev_config_test;
void initialize_dev_config(dev_config_t *config) 
{

    // Initialize user fields
    strncpy(config->WIFI_SSID, DEFAULT_WIFI_SSID, WIFI_SSID_MAX_LEN);
    strncpy(config->WIFI_PASSWORD, DEFAULT_WIFI_PASSWORD, WIFI_PASSWORD_MAX_LEN);
    strncpy(config->APN, DEFAULT_APN, APN_MAX_LEN);
    strncpy(config->GPRS_USER, DEFAULT_GPRS_USER, GPRS_USERNAME_MAX_LEN);
    strncpy(config->GPRS_PASSWORD, DEFAULT_GPRS_PASSWORD, GPRS_PASSWORD_MAX_LEN);
    strncpy(config->SIM_PIN, DEFAULT_SIM_PIN, SIM_PIN_MAX_LEN);
    strncpy(config->BROKER_ADDRESS, DEFAULT_BROKER_ADDRESS, BROKER_ADDR_MAX_LEN);
    strncpy(config->BROKER_USERNAME, DEFAULT_BROKER_USERNAME, BROKER_USERNAME_MAX_LEN);
    strncpy(config->BROKER_PASSWORD, DEFAULT_BROKER_PASSWORD, BROKER_PASSWORD_MAX_LEN);
    strncpy(config->BROKER_PORT, DEFAULT_BROKER_PORT, PORT_MAX_LEN);
    config->mode = MODE_WIFI; 
    config->keypad_enable = false;
    config->capture_image = false;
    config->retrieve_image = false;
    config->update_passcode = false;
    config->passcode_unlock = false;
    config->read_lock_status = false;
    config->config_updated_in_nvs = false;

    // Initialize config fields
    strncpy(config->AES, DEFAULT_AES, AES_KEY_LEN);
    strncpy(config->SERIAL_NUM, DEFAULT_SERIAL_NUM, SERIAL_NUM_LEN + 1);

    
    return;
}
void clear_event_flags(event_flags_t *e_flags)
{
    e_flags->update_wifi_cred             = false;
    e_flags->device_serial_number_updated = false;
    e_flags->update_structure_in_nvs      = false;
    e_flags->factory_reset                = false;
}

void print_config(dev_config_t *config)
{
    // Initialize user fields
    Serial.print("WIFI_SSID: ");
    Serial.println(config->WIFI_SSID);

    Serial.print("WIFI_PASSWORD: ");
    Serial.println(config->WIFI_PASSWORD);

    Serial.print("APN: ");
    Serial.println(config->APN);

    Serial.print("GPRS_USER: ");
    Serial.println(config->GPRS_USER);

    Serial.print("GPRS_PASSWORD: ");
    Serial.println(config->GPRS_PASSWORD);

    Serial.print("SIM_PIN: ");
    Serial.println(config->SIM_PIN);

    Serial.print("BROKER_ADDRESS: ");
    Serial.println(config->BROKER_ADDRESS);

    Serial.print("BROKER_USERNAME: ");
    Serial.println(config->BROKER_USERNAME);

    Serial.print("BROKER_PASSWORD: ");
    Serial.println(config->BROKER_PASSWORD);

    Serial.print("BROKER_PORT: ");
    Serial.println(config->BROKER_PORT);

    // Initialize config fields
    Serial.print("AES: ");
    Serial.println(config->AES);

    Serial.print("SERIAL_NUM: ");
    Serial.println(config->SERIAL_NUM);

    Serial.print("CONFIG_UPDATE_STATUS: ");
    Serial.println(config->config_updated_in_nvs);

}
void check_for_factory_reset(dev_config_t *config,event_flags_t *event_flag)
{
    if(event_flag->factory_reset)
    {
        Serial.println("Doing factory reset");
        initialize_dev_config(config);
        write_default_config_to_nvs(config);
        event_flag->factory_reset = false;
        event_flag->apply_factory_reset = true;
    }

}
void setup(){
    Serial.begin(9600);
    write_passwords_to_nvs();

    #ifndef TEST_WITHOUT_CAMERA
    // SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, CS);
    cam_init(); 
    #else
    // save_dummy_image_to_nvs();
    #endif
//    erase_image_sector();
//    erase_configurations_from_nvs();
    if(is_nvs_stored_config_updated(&dev_config_param))
    {
        Serial.println("Updated structure found from memory");
        Serial.println("Configurations read from nvs: ");
        print_config(&dev_config_param);
    }
    else 
     {
        memset(&dev_config_param,0,sizeof(dev_config_param));
        initialize_dev_config(&dev_config_param);
        write_default_config_to_nvs(&dev_config_param);
        Serial.println("Default Configurations: ");
        print_config(&dev_config_param);
    }
    clear_event_flags(&process_event);
    ble_init(&dev_config_param, &process_event);
}

void loop(){
    if((dev_config_param.mode != MODE_BLE) && (dev_config_param.mode != MODE_WIFI) && (dev_config_param.mode != MODE_LTE)) 
    {
        //Check if valid mode is fetched from nvs to avoid invalid config
        Serial.println("Device's mode is not valid");
        Serial.println("Erase the configurations from nvs");
        erase_configurations_from_nvs();
        Serial.println("Rebooting...");
        ESP.restart();
    }
    else
    {
        check_for_factory_reset(&dev_config_param,&process_event);
        apply_system_mode(&dev_config_param,&process_event);

        if(dev_config_param.keypad_enable == true)
        {
            if(print_once)
            {
                Serial.println("Enter 6 digit password. Starting with *#");
                print_once = false;
            }

            if(get_password_from_keypad())
            {
                if(is_password_correct())
                {
                    Serial.println("Entered password is Correct");
                    unlock_controller();
                    #ifndef TEST_WITHOUT_CAMERA
                    Capture_Image(); 
                    save_image_to_nvs();
                    #endif
                } 
                else
                    Serial.println("Entered password is not found");

            }
        }
    }
    
    
}


