#pragma once
#ifndef model_h
    #define model_h

#ifndef Arduino_h
  #include <Arduino.h>
#endif
#ifndef utils_eeprom_h
  #include "utils_eeprom.h"
#endif
enum MessageType {
  MessageNoData   = 0,
  MessageText     = 1,
  MessageQuery    = 2,
  MessageLocation = 3,
  MessageContact  = 4
};
struct TBBot {
  int32_t  id;
  bool     isBot;
  const char*   firstName;
  const char*   username;
};
struct TBUser {
  int32_t       id;
  bool          isBot;
  const char*   firstName;
  const char*   lastName;
  const char*   username;
  const char*   languageCode;
};
struct TBContact {
  const char*  phoneNumber;
  const char*  firstName;
  const char*  lastName;
  int32_t id;
  const char*  vCard;
};
struct TBMessage {
  int32_t     messageID;
  TBUser      user;
  int32_t     date;
  String      text;
  int32_t     chatInstance;
  const char* callbackQueryData;
  const char* callbackQueryID;
  TBContact   contact;
  MessageType messageType;
};
struct WIFImodelSTA
{
    String    ssid;
    String    password;
};
struct StatusWEB
{
    bool    conectWifi;
    bool    conectWeb;
    bool    conectConfig;
    bool    button1vez;
};

struct UserAcount 
{
  int32_t account[USER_ALL];
};

struct UserAccName 
{
  String account[USER_ALL];
};

struct regUser
{
  uint32_t userIdReg;
  String userNameReg;
};


extern TBUser    userBot;
extern TBMessage userMsg;
extern UserAcount cuentaUser;
extern UserAccName cuentaUserName;
extern regUser regUserStore[USER_ALL];


extern uint32_t countRequest;
//extern uint32_t countStatusLed;
#endif