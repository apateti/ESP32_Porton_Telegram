//#pragma once
#include <WiFi.h>
//#include <WebServer.h>
#include <ArduinoJson.h>
#ifndef header_h
  #include "header.h"
#endif
#define  DEBUG_MODE 1
#ifndef commandWifi_h
    #include "commandWifi.h"
#endif
#ifndef model_h
    #include "model.h"
#endif
WebServer server(80);


bool conectAP(String ssid, String pass){
  Serial.println("\nRutina conectAP");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  Serial.println("Configurado WIFI_STA");
  //ssid = wifiEeprom.readSSID();
  //ssid = readSSID();
  if(ssid == ""){
    return false;
  }
  //pass = wifiEeprom.readPASS();
  //pass = readPASS();
  if(pass == ""){
    WiFi.begin(ssid.c_str());
  }else{
    WiFi.begin(ssid.c_str(), pass.c_str());
  }
  #if DEBUG_MODE > 0
    Serial.println("Conectando...");
  #endif
  uint8_t c = 0;
  while(WiFi.status() != WL_CONNECTED && c < 20) {
    c++;
    delay(500);
    #if DEBUG_MODE > 0
      Serial.print(".");
    #endif
    
  }
  if( c > (20-1)){
    return (false);
  }else{
    #if DEBUG_MODE > 0
      Serial.println("");
      Serial.print("Conectado a la Red Wifi con IP: ");
      Serial.println(WiFi.localIP());
    #endif
    return (true);
  }
}

const void* verCmdoJson();
 
void configServer(){
  server.on("/cmdoJson",verCmdoJson);
  server.begin();
}


bool serverJsonIsOK(){
    if (server.hasArg("plain") == false){
        String body = server.arg("plain");
        Serial.println("\nJson Rx:");
        Serial.println(body);
        return false;
    }else{
         String body = server.arg("plain");
        Serial.println("\nJson Rx:");
        Serial.println(body);
        return true;
    }
    
}

void requestError(){
    // String jsonString;
    // String ssidConfig;
    // String passConfig;
    if (server.hasArg("plain") == false){
        String strJson;
        strJson = "{\"error\": 1";
        strJson += "}";
        server.send(200,"application/json",strJson);
        return;
    }
}

int verCmdo(String& ssidConfig, String& passConfig, String& tokenBot){
    StaticJsonDocument<250> jsonDocument;
    String body = server.arg("plain");
    Serial.println("\nJson Rx:");
    Serial.println(body);
    deserializeJson(jsonDocument, body);
    if(jsonDocument["cmdo"] == "toggle"){
      if(jsonDocument["data"]["userName"]){
        userName = jsonDocument["data"]["userName"].as<String>();
      }else{
        userName = "";
      }
      if(jsonDocument["data"]["userID"]){
        userID = jsonDocument["data"]["userID"];
      }else{
        userID = 0;
      }
      return ToggleLED;
    }else if(jsonDocument["cmdo"] == "accPoint"){
      if(jsonDocument["data"]["userName"]){
        userName = jsonDocument["data"]["userName"].as<String>();
      }else{
        userName = "";
      }
      if(jsonDocument["data"]["userID"]){
        userID = jsonDocument["data"]["userID"];
      }else{
        userID = 0;
      }
      return AccPoint;
    }else if(jsonDocument["cmdo"] == "conectAP"){
      if(jsonDocument["data"]["userName"]){
        userName = jsonDocument["data"]["userName"].as<String>();
      }else{
        userName = "";
      }
      if(jsonDocument["data"]["userID"]){
        userID = jsonDocument["data"]["userID"];
      }else{
        userID = 0;
      }
      Serial.print("\nSSID Recibido del Json: ");
      Serial.println(jsonDocument["data"]["ssid"].as<String>());
      Serial.print("PASS Recibido del Json: ");
      Serial.println(jsonDocument["data"]["pass"].as<String>());
      if(jsonDocument["data"]["ssid"]){
        ssidConfig = jsonDocument["data"]["ssid"].as<String>();
        if(jsonDocument["data"]["pass"]){
          passConfig = jsonDocument["data"]["pass"].as<String>();
        }else{
          passConfig = "";
        }
      return ConectAP;
      }else{
          return  NoComd;
      }
    }else if(jsonDocument["cmdo"] == "tokenBot"){
      // if(jsonDocument["data"]["userName"]){
      //   adminUser = jsonDocument["data"]["userName"].as<String>();
      // }else{
      //   adminUser = "";
      // }
      // if(jsonDocument["data"]["userID"]){
      //   admidId = jsonDocument["data"]["userID"];
      // }else{
      //   admidId = 0;
      // }
      tokenBot = jsonDocument["data"]["token"].as<String>();
      return tokenJson;
    }else if(jsonDocument["cmdo"] == "erraseEE"){
      if(jsonDocument["data"]["userName"]){
        adminUser = jsonDocument["data"]["userName"].as<String>();
      }else{
        adminUser = "";
      }
      if(jsonDocument["data"]["userID"]){
        admidId = jsonDocument["data"]["userID"];
      }else{
        admidId = 0;
      }
      return erraseAll;
    }else if(jsonDocument["cmdo"] == "verAllUser"){
      if(jsonDocument["data"]["userName"]){
        adminUser = jsonDocument["data"]["userName"].as<String>();
      }else{
        adminUser = "";
      }
      if(jsonDocument["data"]["userID"]){
        admidId = jsonDocument["data"]["userID"];
      }else{
        admidId = 0;
      }
      return verUser;
        // }
        // else if(jsonDocument["cmdo"] == "grabarUser"){
        //   if(jsonDocument["data"]["userName"]){
        //     adminUser = jsonDocument["data"]["userName"].as<String>();
        //   }else{
        //     adminUser = "";
        //   }
        //   if(jsonDocument["data"]["userID"]){
        //     admidId = jsonDocument["data"]["userID"];
        //   }else{
        //     admidId = 0;
        //   }
        //   userMsg.user.id = jsonDocument["data"]["user"];
        //   return grabUser;
    }else if(jsonDocument["cmdo"] == "registrar"){
      userName = jsonDocument["data"]["userName"].as<String>();
      userID = jsonDocument["data"]["userID"];
      adminUser = jsonDocument["data"]["adminName"].as<String>();
      admidId = jsonDocument["data"]["adminID"];
      return registUser;
    }else if(jsonDocument["cmdo"] == "verRegistro"){
      if(jsonDocument["data"]["userName"]){
        adminUser = jsonDocument["data"]["userName"].as<String>();
      }else{
        adminUser = "";
      }
      if(jsonDocument["data"]["userID"]){
        admidId = jsonDocument["data"]["userID"];
      }else{
        admidId = 0;
      }
      return verRegistro;
    }else if(jsonDocument["cmdo"] == "erraseUser"){
      userName = jsonDocument["data"]["userName"].as<String>();
      userID = jsonDocument["data"]["userID"];
      adminUser = jsonDocument["data"]["adminName"].as<String>();
      admidId = jsonDocument["data"]["adminID"];
      regID = jsonDocument["data"]["regID"];
      return erraseUser;
    }
}

void sendRequest(String strJson){
    server.send(200,"application/json",strJson);
}

void verClient(){
  server.handleClient();
}