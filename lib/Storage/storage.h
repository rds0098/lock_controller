#ifndef _STORAGE_H
#define _STORAGE_H

#include <Arduino.h>
#include <ardu_cam.h>
#include "./../config.h"

#define SECTOR_SIZE        4096
// #define PASS_NUM_LENGTH    7     // length of a singal password
// #define MAX_NUM_PASS       10    // total number of passwords

const int NVM_Offset = 0x9000;
const int passcodes_address      = NVM_Offset;                     //passwords are stored in the first sector
const int image_address          = NVM_Offset + SECTOR_SIZE;  
const int image_address_sector_1 = image_address;           //next three sectors reserved for image
const int image_address_sector_2 = image_address_sector_1 + SECTOR_SIZE;   
const int image_address_sector_3 =  image_address_sector_2 + SECTOR_SIZE;         
const int system_config_address  = NVM_Offset + SECTOR_SIZE*4; //Last sector for config structure storage

const int password_length = 6;




void write_passwords_to_nvs(void);
//void read_from_nvs(uint8_t *password,size_t size);
bool is_password_correct(void);
bool is_rcv_password_correct(char *str);
//bool update_password_in_nvs(char *password);

bool save_image_to_nvs(void);
void save_dummy_image_to_nvs(void);

uint8_t * read_image_from_nvs(void);

void write_configurations_to_nvs(dev_config_t *config);
bool read_configurations_from_nvs(dev_config_t *config);
void write_updated_config_to_nvs(dev_config_t *dev_config,event_flags_t *process_event);
void write_default_config_to_nvs(dev_config_t *dev_config);
bool is_nvs_stored_config_updated(dev_config_t *dev_config);
void erase_configurations_from_nvs(void);
void is_config_sector_cleared(void);
void erase_image_sector(void);
#endif