#include <Arduino.h>
#include <Arducam_Mega.h>
#include "ardu_cam.h"
#include "lte.h"
#include <SPI.h>

uint8_t imageData = 0;
uint8_t imageDataNext = 0;
uint8_t headFlag = 0;
//const size_t bufferSize = 96*96;
uint8_t buffer[IMG_BUFFER_SIZE] = {0};

CAM_IMAGE_MODE imageMode = CAM_IMAGE_MODE_96X96;
//CAM_IMAGE_MODE imageMode = CAM_IMAGE_MODE_QVGA;
Arducam_Mega myCAM(CS);

static void getImageData(void)
{
    int i = 0;
    Serial.println("Getting image data");
    memset(buffer,0,sizeof(buffer));
    while (myCAM.getReceivedLength())
    {
        imageData = imageDataNext;
        imageDataNext = myCAM.readByte();
        if (headFlag == 1)
        {
            buffer[i++]=imageDataNext;
        //    Serial.print(buffer[i],HEX);  
            if (i >= IMG_BUFFER_SIZE)
            {
                i = 0;
            }
        }
        if (imageData == 0xff && imageDataNext ==0xd8)
        {
            headFlag = 1;
            buffer[i++]=imageData;
            buffer[i++]=imageDataNext;  
        }
        if (imageData == 0xff && imageDataNext ==0xd9)
        {
            headFlag = 0;
            i = 0;
            break;
        }
    }
    

}
void Capture_Image(void)
{
    int length;
    Serial.println("Capture Image");

     myCAM.reset();
     delay(10);
    if(myCAM.takePicture(imageMode,CAM_IMAGE_PIX_FMT_JPG) == CAM_ERR_SUCCESS)
    {
        Serial.println("Image is captured");
        // delay(100);
        getImageData(); //save image data in buffer
        if(buffer[0] == 0xFF && buffer[1] == 0xD8)
        {
            Serial.println("Valid data is received");
            // for(int i = 0; i < IMG_BUFFER_SIZE; i++)
            // {
            //     Serial.print(buffer[i],HEX);
            // }
        }
    }
    else
        Serial.println("ERROR in taking picture");
    
    
    //length = myCAM.getTotalLength();
 
    //Serial.print("Length is: ");
    //Serial.println(length);
    
    // for (int i = 0; i<length; i++)
    // {
    //     Serial.print(buffer[i]);
    // }
/*
    for(int i = 0; i< IMG_BUFFER_SIZE; i++)
    {
        Serial.println(buffer[i]);
    }
*/
}

void cam_init(void)
{
    Serial.println("Init camera");
    if(myCAM.begin() == CAM_ERR_SUCCESS) 
    {
        Serial.println("Camera initialized successfully");
    }

}

