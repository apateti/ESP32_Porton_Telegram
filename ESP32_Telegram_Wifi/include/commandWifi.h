#pragma once
#ifndef commandWifi_h
    #define commandWifi_h

#ifndef Arduino_h
  #include <Arduino.h>
#endif
#include <WebServer.h>

#define NoComd    0
#define ToggleLED 1
#define AccPoint  2
#define ConectAP  3
#define tokenJson 4
#define verUser   5
#define erraseAll 6
#define grabUser  7
#define registUser 8
#define verRegistro 9
#define erraseUser 10

bool conectAP(String ssid, String pass);
void configServer();
bool serverJsonIsOK();
void requestError();
int verCmdo(String& ssidConfig, String& passConfig, String& tokenBot);
void sendRequest(String strJson);
void verClient();




#endif