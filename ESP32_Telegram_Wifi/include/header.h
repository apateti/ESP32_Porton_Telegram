#pragma once
#ifndef header_h
    #define header_h

#define DEBUG_MODE 1            //Si es >0 se Activa Debug, se imprime por  Terminalls Logs
#define BUFFER_BIG 2000         //Tamaño del Buffer para Json
//#define EEPROM_SIZE 1024      //Tamaño del Buffer para la EEPROM
#define statusLED 2
#define bottonProg 0             //Led de la Tarjeta ESP32 DEV
#define LED 15

extern String ssid;
extern String pass;

extern String userName;
extern int32_t userID;
extern String adminUser;
extern int32_t admidId;
extern uint8_t regID;


extern TaskHandle_t TaskWifi;

// uint16_t countStatusLed;
// bool flagConfig = false;
// bool flag1Vez = true;
//uint16_t countProg = 0;

// void readAllUser(void);

#endif