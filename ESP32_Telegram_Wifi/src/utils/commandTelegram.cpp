//#pragma once

#include "commandTelegram.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#ifndef model_h
    #include "model.h"
#endif

#define TOKEN_BOT       "1163113539:AAEZfgENIoMnjdzN3kdBrqZiCGMjjC_ZcdU"

//extern String tokenBot; //         = "1163113539:AAEZfgENIoMnjdzN3kdBrqZiCGMjjC_ZcdU";
String urlPing          = "api.telegram.org";
String urlTelegram      = "https://api.telegram.org/bot";
String comandGetUpdate  = "/getUpdates?offset=";
String comandGet        = "/getUpdates";
int32_t update_id       = 1;
String comandGetMe      = "/getMe";
String comandSendText   = "/sendMessage";
String comandAnswQuery  = "answerInlineQuery";
//
// String CommadSTART      = "/start";
// String CommadREGISTRO   = "/registrar";
// String CommadMENU       = "/menu";
// String CommadLED_ON     = "LED_ON";
// String CommadLED_OFF    = "LED_OFF";
// String CommadLED_Toggle = "LED_Toggle";
// String CommadLED_Status = "Estatus_LED";
// String CommadOption     = "/opciones";
//query Keyboard
// String queryON          = "LedOn";
// String queryOFF         = "LedOff";
// String queryToggle      = "LedToggle";
// String queryStatus      = "LedStatus";

#define BUFFER_BIG 2000
DynamicJsonDocument root(BUFFER_BIG);

int32_t  getUpdateID = 0;
int32_t  updateID = 0;


int verBot(String botToken){
  Serial.println("Enviando GetMe");
  Serial.println(botToken);
  HTTPClient http;
  String serverPath = urlTelegram;
  serverPath += botToken;
  serverPath += comandGetMe;
  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0){
    String payload = http.getString();
    deserializeJson(root, payload);
    if(root["ok"]){
      userBot.id = root["result"]["id"];
      userBot.firstName = root["result"]["first_name"];
      userBot.username = root["result"]["username"];
    }
  }
  http.end();
  //Serial.print("Respuesta GetMe: "); Serial.println(httpResponseCode);
  return (httpResponseCode);
}

int requestGetUpdateClear(String token){
  #if DEBUG_MODE > 0
    //Serial.println("\nFuncion requestGetUpdateClear");
    // Serial.print("Mensaje ID: ");
    // Serial.println(getUpdateID);
    // Serial.print("Token: ");
    // Serial.println(token);
    //Serial.print("Solicitud Nro: ");
    //Serial.println(countRequest);
    //countRequest++;
  #endif
  int updateID = getUpdateID + 1;
  
  HTTPClient http;
  String serverPath = urlTelegram;
  serverPath += token;
  serverPath += comandGetUpdate;
  serverPath += updateID;
  //Serial.println(serverPath);
  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0){
    String payload = http.getString();
    // #if DEBUG_MODE > 0
    //   Serial.println(payload);
    // #endif
    deserializeJson(root, payload);
    //userMsg.messageID = root["result"][0]["update_id"];;
    int32_t  updateID = root["result"][0]["update_id"];
    if (updateID == 0){
      #if DEBUG_MODE > 0
        Serial.print("\nBot sin Mensajes");
      #endif
      userMsg.messageType = MessageNoData;
      // Serial.println("Respuesta: ");
      // Serial.println(userMsg.messageType);
    }else{
      if(root["result"][0]["callback_query"]["id"]){
        #if DEBUG_MODE > 0
          Serial.print("\nMensaje callback_query ");
        #endif
        userMsg.messageType = MessageQuery;
        userMsg.messageID = root["result"][0]["update_id"];
        getUpdateID = root["result"][0]["update_id"];
        userMsg.user.id = root["result"][0]["callback_query"]["from"]["id"];
        userMsg.user.username = root["result"][0]["callback_query"]["from"]["username"];
        userMsg.user.firstName = root["result"][0]["callback_query"]["from"]["first_name"];
        userMsg.user.lastName = root["result"][0]["callback_query"]["from"]["last_name"];
        userMsg.text = root["result"][0]["callback_query"]["message"]["text"].as<String>();
        userMsg.callbackQueryData = root["result"][0]["callback_query"]["data"];   
        userMsg.callbackQueryID = root["result"][0]["callback_query"]["id"];
      }else if(root["result"][0]["message"]["message_id"]){
        userMsg.messageType = MessageText;
        userMsg.messageID = root["result"][0]["update_id"];
        getUpdateID = root["result"][0]["update_id"];
        userMsg.user.id = root["result"][0]["message"]["from"]["id"];
        userMsg.user.username = root["result"][0]["message"]["from"]["username"];
        userMsg.user.firstName = root["result"][0]["message"]["from"]["first_name"];
        userMsg.user.lastName = root["result"][0]["message"]["from"]["last_name"];
        userMsg.text = root["result"][0]["message"]["text"].as<String>();
      }
    }
  }
  http.end();
  return (httpResponseCode);
}

int requestGetUpdate(String token){
  HTTPClient http;
  String serverPath = urlTelegram;
  serverPath += token;
  serverPath += comandGet;
  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode>0){
    String payload = http.getString();
    root.clear();
    deserializeJson(root, payload);
    #if DEBUG_MODE > 0
      // Serial.println("\nPayload desde requestGetUpdate: ");
      // Serial.println(payload);
    #endif
    userMsg.messageID = root["result"][0]["update_id"];;
    if (userMsg.messageID == 0){
      #if DEBUG_MODE > 0
        Serial.print("Desde requestGetUpdate Bot sin Mensajes");
      #endif
      userMsg.messageType = MessageNoData;
      getUpdateID = 1;
    }else{
      #if DEBUG_MODE > 0
        Serial.print("\nDesde requestGetUpdate Bot con Mensajes");
      #endif
      if(root["result"][0]["callback_query"]["id"]){
        #if DEBUG_MODE > 0
          Serial.print("\nMensaje callback_query ");
        #endif
        userMsg.messageType = MessageQuery;
      }else if(root["result"][0]["message"]["message_id"]){
        userMsg.messageType = MessageText;
        userMsg.messageID = root["result"][0]["update_id"];
        getUpdateID = root["result"][0]["update_id"];
        userMsg.user.id = root["result"][0]["message"]["from"]["id"];
        userMsg.user.username = root["result"][0]["message"]["from"]["username"];
        userMsg.user.firstName = root["result"][0]["message"]["from"]["first_name"];
        userMsg.user.lastName = root["result"][0]["message"]["from"]["last_name"];
        userMsg.text = root["result"][0]["message"]["text"].as<String>();
        
      }
    }
  }
  #if DEBUG_MODE > 0
    Serial.print("\nDesde requestGetUpdate getUpdateID: ");
    Serial.println(getUpdateID);
  #endif
  http.end();
  return (httpResponseCode);
}

int sendComand(int32_t userId, String mensaje){
  HTTPClient http;
  String json;
  StaticJsonDocument<1000> root;
  root.clear();
  root["chat_id"] = userId;
  root["text"] = mensaje;
  serializeJson(root, json);
  #if DEBUG_MODE > 0
    // Serial.print("\nJson a Tx:");
    // Serial.println(json);
  #endif
  String serverName = urlTelegram;
  serverName += tokenBot;
  serverName += comandSendText;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(json);
  //Serial.print("HTTP Response code de SendText: ");
  //Serial.println(httpResponseCode);
  http.end();
  return (httpResponseCode);
}

int sendMenu(int32_t userId){
  HTTPClient http;
  String param((char *)0);
  param.reserve(512);
  String json;
  //json = "\"reply_markup\":{\"keyboard\":[[\"LED_ON\",\"LED_OFF\",\"Toggle_LED\"],[\"Estatus_LED\"]],\"resize_keyboard\":true}";
  json = "{\"keyboard\":[[\"LED_ON\",\"LED_OFF\",\"LED_Toggle\"],[\"Estatus_LED\"]],\"resize_keyboard\":true}";
  StaticJsonDocument<1000> root;
  root.clear();
  root["chat_id"] = userId;
  root["text"] = "Seleccione un Opción";
  DynamicJsonDocument doc(512);
  deserializeJson(doc, json);
  JsonObject myKeyb = doc.as<JsonObject>();
  root["reply_markup"] = myKeyb;
  serializeJson(root, param);
  #if DEBUG_MODE > 0
    Serial.print("\nJSON para sendMenu: ");
    Serial.println(param);
  #endif 
  String serverName = urlTelegram;
  serverName += tokenBot;
  serverName += comandSendText;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  //http.addHeader("reply_markup");
  int httpResponseCode = http.POST(param);
  http.end();
  #if DEBUG_MODE > 0
    Serial.println("");
    Serial.print("Respuesta de sendMenu: ");
    Serial.println(httpResponseCode);
  #endif
  return (httpResponseCode);
}