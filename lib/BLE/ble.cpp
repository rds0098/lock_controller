#include <ble.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <storage.h>
#include <lock_control.h>
#include "ardu_cam.h"
#include "./../config.h"
#include "data_parser.h"

bool ble_connected = false;
dev_config_t *dev_config;
event_flags_t *process_event_flag;

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer)
    {
        ble_connected = true;
        Serial.println("BLE Connected");
    };
    void onDisconnect(BLEServer* pServer)
    {
        ble_connected = false;
        Serial.println("BLE Disconnected");
    }
};

class USERCharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic* pCharacteristic)
    {
        uint8_t *rcv_data = pCharacteristic->getData();

        Serial.println((char*)rcv_data);

        // Call data parsar to retteive device settings parameters
        parseDevSettingsData(dev_config, rcv_data, process_event_flag);
    };

    void onRead(BLECharacteristic* pCharacteristic)
    {
        if(dev_config->retrieve_image = true)
        {
            //send image data here      
            uint8_t * image = read_image_from_nvs();
            for(int i = 0; i < IMG_BUFFER_SIZE/BLE_MAX_DATA_BYTES; i++)
            {
                pCharacteristic->setValue(image + i,BLE_MAX_DATA_BYTES);

            }
            pCharacteristic->setValue(image + (BLE_MAX_DATA_BYTES * (IMG_BUFFER_SIZE/BLE_MAX_DATA_BYTES)), IMG_BUFFER_SIZE - (BLE_MAX_DATA_BYTES * (IMG_BUFFER_SIZE/BLE_MAX_DATA_BYTES)));

            Serial.println("Image data send over ble");
            dev_config->retrieve_image = false;
        }
    }
};
class CONFIGCharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic* pCharacteristic)
    {
        uint8_t *rcv_data = pCharacteristic->getData();

        Serial.println((char*)rcv_data);

        // Call data parsar to reteive configuration parameters
        parseDevConfigData(dev_config, rcv_data,process_event_flag);

    }
};

void ble_init(dev_config_t *device_setting,event_flags_t *p_events)
{
    dev_config = device_setting;
    process_event_flag = p_events;

    BLEDevice::init("Lock_Controller");
    Serial.println("BLE is initilaized");
    BLEServer *pServer = BLEDevice::createServer();

    // Set up security for the BLE server
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
    pSecurity->setCapability(ESP_IO_CAP_OUT);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pSecurity->setStaticPIN(APP_PAIRING_PASS_CODE);  // Set your desired static PIN here

    // adding services and characterisitce for Smar Locker
    pServer -> setCallbacks(new ServerCallbacks());

    BLEService *pService_1 = pServer->createService(USER_SERVICE_UUID);
    BLEService *pService_2 = pServer->createService(CONFIG_SERVICE_UUID);

    BLECharacteristic *pCharacteristic_user = pService_1->createCharacteristic(
                                                            USER_CHARACTERISTIC_UUID,
                                                            BLECharacteristic::PROPERTY_WRITE |
                                                            BLECharacteristic::PROPERTY_READ
                                                            );
    BLECharacteristic *pCharacteristic_config = pService_2->createCharacteristic(
                                                                    CONFIG_CHARACTERISTIC_UUID,
                                                                    BLECharacteristic::PROPERTY_WRITE 
                                                                    );
    pCharacteristic_user->setCallbacks(new USERCharacteristicCallbacks());
    pCharacteristic_config->setCallbacks(new CONFIGCharacteristicCallbacks());
    pService_1->start();
    pService_2->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(USER_SERVICE_UUID);
    pAdvertising->addServiceUUID(CONFIG_SERVICE_UUID);
    BLEDevice::startAdvertising();
}

