#ifndef _ARDU_CAM_H
#define _ARDU_CAM_H

//#define IMG_BUFFER_SIZE 96*96
//#define IMG_BUFFER_SIZE  4096
//#define IMG_BUFFER_SIZE  1152
//#define IMG_BUFFER_SIZE  18000
#define IMG_BUFFER_SIZE  1024

//const int CameraPin = 15;
const int CS = 33;
const int SPI_MOSI = 32;
const int SPI_MISO = 19;
const int SPI_SCK = 18;
//const int CS = 35;
void Capture_Image(void);
void cam_init(void);
#endif