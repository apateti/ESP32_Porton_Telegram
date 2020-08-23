//#pragma once
#include "utils_eeprom.h"
#include <EEPROM.h>
#ifndef model_h
    #include "model.h"
#endif

bool EEPROMinit(){
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM");
    return (false);
  }else{
    Serial.println("OK to initialise EEPROM");
    return (true);
  }
}

void erraseALL(void){
    for (int i = 0; i < EEPROM_SIZE; i++){
        EEPROM.write(i, 0xFF);
    }
    EEPROM.commit();
}

String readSSID(){
  String ssidSTA;
  char in[32];
  char curIn;
  curIn = EEPROM.read(SSID_ADDR);
  if(curIn == 0xFF || curIn == 0x00){
    Serial.println("SSID[0] es 0 o FF");
    ssidSTA = "";
    return (ssidSTA);
  }
  Serial.println("read SSID: ");
  for (int i = 0; i < SSID_SIZE; i++){
    curIn = EEPROM.read(i);
    in[i] = curIn;
    if(curIn == 0x00)
      break;
    Serial.print(in[i]);
  }
  ssidSTA = String(in);
  return (ssidSTA);
}
String readPASS(){
  String passSTA;
  char in[PASS_SIZE];
  char curIn;
  curIn = EEPROM.read(PASS_ADDR);
  if(curIn == 0xFF || curIn == 0x00){
    Serial.println("PASS[0] es 0 o FF");
    passSTA = "";
    return (passSTA);
  }
  Serial.println("\nread PASS: ");
  for (int i = 0; i < PASS_SIZE; i++){
    curIn = EEPROM.read(i + PASS_ADDR);
    in[i] = curIn;
    if(curIn == 0x00)
      break;
    Serial.print(curIn);
  }
  passSTA = String(in);
  return (passSTA);
}

void writeSSID(String ssidSTA){
  if(ssidSTA == ""){
    return;
  }
  Serial.print("write SSID: ");
  for (int i = 0; i < SSID_SIZE; i++){
    EEPROM.write(i, ssidSTA[i]);
    if(ssidSTA[i] == 0x00)
      break;
    Serial.print(ssidSTA[i]);
  }
  EEPROM.commit();
}

void writePASS(String passSTA){
  if(passSTA == ""){
    return;
  }
  Serial.print("\nwrite PASS: ");
  for (int i = 0; i < PASS_SIZE; i++){
    EEPROM.write(i + PASS_ADDR, passSTA[i]);
    if(passSTA[i] == 0x00)
      break;
    Serial.print(passSTA[i]);
  }
  EEPROM.commit();
}

String readTOKEN(){
  String tokenBot;
  char in[TOKEN_SIZE+1];
  char curIn;
  curIn = EEPROM.read(TOKEN_ADDR);
  if(curIn == 0xFF || curIn == 0x00){
    Serial.println("tokenBot[0] es 0 o FF");
    tokenBot = "";
    return (tokenBot);
  }
  Serial.println("read TOKEN: ");
  for (int i = 0; i < TOKEN_SIZE; i++){
    curIn = EEPROM.read(i + TOKEN_ADDR);
    in[i] = curIn;
    if(curIn == 0x00)
      break;
    Serial.print(curIn);
  }
  in[TOKEN_SIZE] = '\0';
  tokenBot = String(in);
  return (tokenBot);
}

void writeTOKEN(String tokenBot){
  if(tokenBot == ""){
    return;
  }
  Serial.print("\nwrite TOKEN: ");
  for (int i = 0; i < TOKEN_SIZE; i++){
    EEPROM.write(i + TOKEN_ADDR, tokenBot[i]);
    if(tokenBot[i] == 0x00)
      break;
    Serial.print(tokenBot[i]);
  }
  EEPROM.commit();
}

void readAllUser(){
    uint32_t valor;
    char caract[5];
    Serial.print("Addr Inicial USER: "); Serial.println(USER_0);
    for (int i = 0; i < USER_ALL; i++)
    {
        caract[0] = EEPROM.read(USER_0+(i*4));
        caract[1] = EEPROM.read(USER_0+(i*4)+1);
        caract[2] = EEPROM.read(USER_0+(i*4)+2);
        caract[3] = EEPROM.read(USER_0+(i*4)+3);
        caract[4] = '\0';
        valor = uint8_t(caract[3])*0x1000000 + uint8_t(caract[2])*0x0010000 + uint8_t(caract[1])*0x0000100 + uint8_t(caract[0]);
        Serial.print("User: "); Serial.print(i); Serial.print(": ");
        Serial.println(valor);
        cuentaUser.account[i] = valor;
    }
}

void writeAllUser(){
    
  uint32_t valor;
  
  for (int i = 0; i < USER_ALL; i++)
  {
    valor = cuentaUser.account[i];
    EEPROM.write((i * 4) + USER_0, uint8_t(valor));
    EEPROM.write((i * 4)+1 + USER_0, uint8_t(valor >> 8));
    EEPROM.write((i * 4)+2 + USER_0, uint8_t(valor >> 16));
    EEPROM.write((i * 4)+3 + USER_0, uint8_t(valor >> 24));
  }
  EEPROM.commit();
}

void readUser_i(int index){
  Serial.println("Rutina readUser_i");
    uint32_t valor;
    char caract[5];
    caract[0] = EEPROM.read(USER_0+(index*4));
    Serial.print(uint8_t(caract[0]));
    Serial.print(',');
    caract[0+1] = EEPROM.read(USER_0+(index*4)+1);
    Serial.print(uint8_t(caract[1]));
    Serial.print(',');
    caract[0+2] = EEPROM.read(USER_0+(index*4)+2);
    Serial.print(uint8_t(caract[2]));
    Serial.print(',');
    caract[0+3] = EEPROM.read(USER_0+(index*4)+3);
    Serial.print(uint8_t(caract[3]));
    Serial.println(',');
    caract[0+4] = '\0';
    valor = uint8_t(caract[3])*0x1000000 + uint8_t(caract[2])*0x0010000 + uint8_t(caract[1])*0x0000100 + uint8_t(caract[0]);
    cuentaUser.account[index] = valor;
    Serial.print("Valor Leido de la EEPROM: ");
    Serial.println(valor);
}

void writeUser_i(int index){
    Serial.println("Rutina writeUser_i");
    uint32_t valor;
    valor = cuentaUser.account[index];
    Serial.println(valor);
    EEPROM.write((index * 4) + USER_0, uint8_t(valor));
    Serial.print(uint8_t(valor));
    Serial.print(',');
    EEPROM.write((index * 4)+1 + USER_0, uint8_t(valor >> 8));
    Serial.print(uint8_t(valor >> 8));
    Serial.print(',');
    EEPROM.write((index * 4)+2 + USER_0, uint8_t(valor >> 16));
    Serial.print(uint8_t(valor >> 16));
    Serial.print(',');
    EEPROM.write((index * 4)+3 + USER_0, uint8_t(valor >> 24));
    Serial.print(uint8_t(valor >> 24));
    Serial.println(',');
    EEPROM.commit();
}

void writeUserName_i(int index, String userName){
    Serial.println("Rutina writeUserName_i");
    char valor[USER_Name_SIZE+1];
    //valor = userName;
    userName.toCharArray(valor, sizeof(valor));
    //valor = cuentaUser.account[index];
    Serial.println(valor);
    for (uint8_t i = 0; i < USER_Name_SIZE; i++)
    {
      EEPROM.write(i + (USER_Name_0 + (USER_Name_SIZE*index)), char(valor[i]));
      if(valor[i] == '\0'){
        break;
      }
    }
    
    // EEPROM.write((index * 4) + USER_Name_0, char(valor[0]));
    // Serial.print(char(valor[0]));
    // Serial.print(',');
    // EEPROM.write((index * 4)+1 + USER_Name_0, char(valor[1]));
    // Serial.print(char(valor[1]));
    // Serial.print(',');
    // EEPROM.write((index * 4)+2 + USER_Name_0, char(valor[1]));
    // Serial.print(char(valor[1]));
    // Serial.print(',');
    // EEPROM.write((index * 4)+3 + USER_Name_0, char(valor[1]));
    // Serial.print(char(valor[1]));
    // Serial.println(',');
    EEPROM.commit();
}

// void writePASS(String passSTA){
//   if(passSTA == ""){
//     return;
//   }
//   Serial.print("\nwrite PASS: ");
//   for (int i = 0; i < PASS_SIZE; i++){
//     EEPROM.write(i + PASS_ADDR, passSTA[i]);
//     if(passSTA[i] == 0x00)
//       break;
//     Serial.print(passSTA[i]);
//   }
//   EEPROM.commit();
// }

void readAllUserName(void){
    String ssidSTA;
    char in[USER_Name_SIZE+1];
    char curIn;
    Serial.print("Addr Inicial USER_Name: "); Serial.println(USER_Name_0);
    for (int i = 0; i < USER_ALL; i++)
    {
      for (uint8_t j = 0; j < USER_Name_SIZE; j++){
        if(j == 0){
          curIn = EEPROM.read(j + (USER_Name_0 + (USER_Name_SIZE*i)));
          if(curIn == 0xFF || curIn == 0x0){
            in[j] = '\0';
            break;
          }else{
            in[j] = curIn;
          }
        }
        curIn = EEPROM.read(j + (USER_Name_0 + (USER_Name_SIZE*i)));
        in[j] = curIn;
        if(curIn == 0)
          break;
      }
      cuentaUserName.account[i] = String(in);
    }
    for (int i = 0; i < USER_ALL; i++){
      Serial.print("User Name "); Serial.print(i); Serial.print(": ");
      Serial.println(cuentaUserName.account[i]);
    }
}

void readAllUserRegPen(void){

}
