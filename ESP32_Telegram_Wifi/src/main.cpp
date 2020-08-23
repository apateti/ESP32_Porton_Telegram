#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Ping.h>


//Se incluyen mis Archivos
#include "header.h"
#include "utils_isr_timer.h"
#include "utils_eeprom.h"
#ifndef commandTelegram_h
  #include "commandTelegram.h"
#endif
#ifndef commandWifi_h
    #include "commandWifi.h"
#endif

bool falgAdmin = false;
bool flagConfig = false;
bool flag1Vez = true;
uint16_t countStatusLed;
String ssid;
String pass;
String tokenBot;
TBUser    userBot;
TBMessage userMsg;
UserAcount cuentaUser;
UserAccName cuentaUserName;
TaskHandle_t TaskWifi;
String userName;
int32_t userID;
String adminUser;
int32_t admidId;
uint8_t regID;
regUser regUserStore[USER_ALL];

void readEepromAllUser(void);
void imprimirAllUser(void);
void verTelegram(void);
void loop2(void * pvParameters);
bool userIsRegistred(void);
void userNoRegister(void);
void enviarRegistrarTel(void);
bool adminRegistra(void);
bool adminRegistraUser(void);
void guardarREgistrarTel(void);
void limpiarRegUser(void);
void enviarRegPend(void);
void enviarUserRequest(void);
bool verRegisterActive(void);
void registoPendiente(void);
bool verCuenta(int32_t scanUserId);
void envRegistroUser(void);

void setup() {
  #if DEBUG_MODE > 0
    Serial.begin(115200); 
  #endif
  initISR_Timer();
  EEPROMinit();
  pinMode(LED, OUTPUT);
  pinMode(statusLED, OUTPUT);
  digitalWrite(LED, LOW);
  digitalWrite(statusLED, LOW);
  pinMode(bottonProg, INPUT);
  ssid = readSSID();
  pass = readPASS();
  bool conec = conectAP(ssid, pass);
  if(conec){
    configServer();
  }
  String tok = readTOKEN();
  if(tok == ""){
    #if DEBUG_MODE > 0
      Serial.println("\nToken leido de la EEPROM nulo");
    #endif
  }
  else if(tok[10] == ':'){
    Serial.println("\nToken de la EEPROM Valido, con token: ");
    Serial.println(tok);
    int request = verBot(tok);
    if(request != 200){
      #if DEBUG_MODE > 0
        Serial.println("\nBot no Responde");
      #endif
    }else{
      #if DEBUG_MODE > 0
        Serial.println("Bot Responde y en Linea");
        Serial.print("\nId del Bot: ");
        Serial.println(userBot.id);
        Serial.print("Nombre del Bot: ");
        Serial.println(userBot.firstName);
        Serial.print("UserName del Bot: ");
        Serial.println(userBot.username);
      #endif
      tokenBot = tok;
      Serial.print("Token Leido EEPROM: ");
      Serial.println(tokenBot);
      //writeTOKEN(tok);
      requestGetUpdate(tokenBot);
    }
  }else{
    #if DEBUG_MODE > 0
      Serial.println("\nToken leido de la EEPROM no valido");
    #endif
  }
  limpiarRegUser();
  readEepromAllUser();
  imprimirAllUser();
  xTaskCreatePinnedToCore(
      loop2, /* Funcion de la tarea1 */
      "Tarea1", /* Nombre de la tarea */
      10000,  /* Tamaño de la pila */
      NULL,  /* Parametros de entrada */
      0,  /* Prioridad de la tarea */
      &TaskWifi,  /* objeto TaskHandle_t. */
      0); /* Nucleo donde se correra */
}
void verButton();
void statuLED();


void loop() {
  // verClient();
  // statuLED();
  // verButton();
  verTelegram();
  // put your main code here, to run repeatedly:
}

void loop2(void * pvParameters){
  Serial.print("loop2 se corre en el nucleo: ");
  Serial.println(xPortGetCoreID());
  while (true)
  {
    verClient();
    statuLED();
    verButton();
  }
}

String scanWifi(void);
bool configWiFi(String ssidSTA, String passSTA);
String respuestaConfig(int code);
bool verToken(void);

//Si se Recibe Peticiones WEB alServidor se ejecuta
//esta Funcion
void verCmdoJson(){
  Serial.print("verCmdo nucleo: ");
  Serial.println(xPortGetCoreID());
  String strJson;
  String  mensaje;
  bool envMsjAdmTel = false;
  if(!serverJsonIsOK()){
    Serial.println("Se Rx con ERROR");
    requestError();
  }else{
    Serial.println("Se Rx SIN ERROR");
    int requeest = verCmdo(ssid, pass, tokenBot);
    if(requeest == ToggleLED){
      if(verCuenta(userID)){
        digitalWrite(LED, !digitalRead(LED));
        String mensaje;
        if(digitalRead(LED))
          mensaje = "ON";
        else
          mensaje = "OFF";
        strJson = "{\"error\": 0, \"LED\":";
        strJson += mensaje;
        strJson += "}";
        envMsjAdmTel = true;
      }else{
        strJson = "{\"error\": 401}";
      }
      sendRequest(strJson);
      if(envMsjAdmTel){
        if(userID != cuentaUser.account[0]){
          mensaje = "El Usuario: ";
          mensaje += userName;
          if(digitalRead(LED))
            mensaje += "\nLED en ON por App";
          else
            mensaje += "\nLED en OFF por App";
          sendComand(cuentaUser.account[0], mensaje);
        }
      }
    }else if(requeest == AccPoint){
      strJson = "";
      strJson = scanWifi();
      sendRequest(strJson);
    }else if(requeest == ConectAP){
      if(configWiFi(ssid, pass)){
        strJson = ""; 
        strJson = respuestaConfig(0);
        sendRequest(strJson);
      }else{
        strJson = ""; 
        strJson = respuestaConfig(1);
        sendRequest(strJson);
      }
    }else if(requeest == tokenJson){
      if(verToken()){
        strJson = "{\"error\": 0}";
        writeTOKEN(tokenBot);
      }else{
        strJson = "{\"error\": 401}";
      }
      sendRequest(strJson);
    }else if(requeest == erraseAll){
      if(admidId == cuentaUser.account[0]){
        strJson = "{\"error\": 0}";
        erraseALL();
      }else{
        strJson = "{\"error\": 401}";
      }
      sendRequest(strJson);
    }else if(requeest == verUser){
      // strJson = "{\"error\": 0}";
      // sendRequest(strJson);
      enviarUserRequest();
      imprimirAllUser();
    }else if(requeest == grabUser){
      strJson = "{\"error\": 0}";
      sendRequest(strJson);
      for (int8_t i = 0; i < USER_ALL; i++){
        if(cuentaUser.account[i] != 0xFFFFFFFF){
          cuentaUser.account[i] = userMsg.user.id;
          Serial.print("User Rx: "); Serial.println(cuentaUser.account[i]);
          writeUser_i(i);
          readUser_i (i);
          Serial.print("Usuario Leido de la EEPROM: ");
          Serial.println(cuentaUser.account[i]);
          break;
        }
      }
      
    }else if(requeest == registUser){
      bool envRegUser = false;
      if(cuentaUser.account[0] == admidId){
        if(adminRegistraUser()){
          strJson = "{\"error\": 0}";
          envRegUser = true;
          //sendRequest(strJson);
        }else{
          strJson = "{\"error\": 400}";
        //sendRequest(strJson);
        }
      }else{
        strJson = "{\"error\": 401}";
        //sendRequest(strJson);
      }
      sendRequest(strJson);
      if(envRegUser){
        envRegistroUser();
      }
    }else if(requeest == verRegistro){
      if(cuentaUser.account[0] == admidId){
        enviarRegPend();
      }else{
        strJson = "{\"error\": 401}";
        sendRequest(strJson);
      }
      
    }else if(requeest == erraseUser){
      readAllUser();
      if(cuentaUser.account[0] == admidId){
        if(userID == cuentaUser.account[regID]){
          cuentaUser.account[regID] = 0xFFFFFFFF;
          writeUser_i(regID);
          writeUserName_i(regID, "");
          strJson = "{\"error\": 0}";
        }else{
          strJson = "{\"error\": 400}";
        }
      }else{
        strJson = "{\"error\": 401}";
      }
      sendRequest(strJson);
    }
    else{
      strJson = "{\"error\": 401}";
      sendRequest(strJson);
    }
    
  }
}

bool verToken(void){
  if(tokenBot[10] == ':'){
    Serial.println("Token de la Json Valido");
    int request = verBot(tokenBot);
    if(request != 200){
      #if DEBUG_MODE > 0
        Serial.println("\nBot no Responde");
      #endif
      return (false);
    }else{
      #if DEBUG_MODE > 0
        Serial.println("Bot Responde y en Linea");
        Serial.print("\nId del Bot: ");
        Serial.println(userBot.id);
        Serial.print("Nombre del Bot: ");
        Serial.println(userBot.firstName);
        Serial.print("UserName del Bot: ");
        Serial.println(userBot.username);
      #endif
      //writeTOKEN(tokenBot);
      requestGetUpdate(tokenBot);
      return (true);
    }

  }else{
    #if DEBUG_MODE > 0
      Serial.println("\nToken leido de la EEPROM no valido");
    #endif
    return (true);
  }
}

String scanWifi(void){
  Serial.print("\nComando Json Buscar Wifi");
  //DynamicJsonDocument jsonBuffer(2048);
  String jsonString;
  #if DEBUG_MODE > 0
    Serial.println("\nEscaneando Redes Wifi...");
  #endif
  int n = WiFi.scanNetworks();
  Serial.print("Redes Encontradas: ");
  Serial.println(n);
  for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
  }
  Serial.println("\n\n");
  if (n == 0){
    String strJson;
    strJson = "{\"error\": 0, \"wifi\":[]}";
    //server.send(200,"application/json",strJson);
    return (strJson);
  }else{
    StaticJsonDocument<2048> root;
    root.clear();
    DynamicJsonDocument doc(1500); 
    doc.clear();
    JsonArray arrayWifi = doc.to<JsonArray>();
    //arrayWifi.clear();
    //JsonObject s1 = doc.as<JsonObject>();
    JsonObject s1 = arrayWifi.createNestedObject();
    s1.clear();
    root["error"] = 0;
    arrayWifi.remove(0);
    for (int i = 0; i < n; ++i){
      s1["ssid"] = WiFi.SSID(i);
      String ssid = s1["wifi"];
      int qos = WiFi.RSSI(i);
      if(qos < -85)
        s1["qos"] = 1;
      else if(qos < -75)
        s1["qos"] = 2;
      else if(qos < -60)
        s1["qos"] = 3;
      else
        s1["qos"] = 4;
      s1["encryption"] = WiFi.encryptionType(i);
      arrayWifi.add(s1);
      Serial.print("Tamaño Arreglo para i= ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(arrayWifi.size());
    }
    Serial.print("Tamaño Arreglo Total: ");
    Serial.println(arrayWifi.size());
    if(arrayWifi.size() > n){
      arrayWifi.remove(0);
    }
    //Serial.println("");
    //Serial.println(arrayWifi);
    root["wifi"] = arrayWifi;
    serializeJson(root, jsonString);
    Serial.print("\n");
    Serial.println(jsonString);
    //server.send(200,"application/json",jsonString);
    return (jsonString);
  }
}

bool configWiFi(String ssidSTA, String passSTA){
//  WiFi.disconnect();
//  WiFi.mode(WIFI_OFF);
//  delay(1000);
//  WiFi.mode(WIFI_STA);
//  WiFi.setAutoReconnect(true);
  if(passSTA  == ""){
    Serial.println("Conectando Sin PASS...");
    WiFi.begin(ssidSTA.c_str());
  }else{
    Serial.println("Conectando Con PASS...");
    WiFi.begin(ssidSTA.c_str(), passSTA.c_str());
  }
  #if DEBUG_MODE > 0
    Serial.println("Conectando...");
    Serial.print("SSID: ");
    Serial.println(ssidSTA);
    Serial.print("PASS: ");
    Serial.println(passSTA);
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
//    wifiEeprom.writeSSID(ssidSTA);
//    wifiEeprom.writePASS(passSTA);
//      writeSSID(ssidSTA);
//      writePASS(passSTA);
    return (true);
  }
  
}

String respuestaConfig(int code){
  StaticJsonDocument<500> root;
  DynamicJsonDocument doc(200); 
  String jsonString;
  root["error"] = code;
  if(code == 0){
    IPAddress broadCast =  WiFi.localIP();
    String ipSTA = String(broadCast[0])+ String(".") + String(broadCast[1])+ String(".") + String(broadCast[2])+ String(".") + String(broadCast[3]);
    doc["dirIP"] = ipSTA;
    root["data"] = doc;
  }else{
    doc["dirIP"] = "";
    root["data"] = doc;
  }
  serializeJson(root, jsonString);
  //server.send(200,"application/json",jsonString);
  Serial.print("\n");
  Serial.println(jsonString);
  return (jsonString);
}

void statuLED(){
  portENTER_CRITICAL(&timerMux);
  bool flagTime = flagTimer;
  portEXIT_CRITICAL(&timerMux);
  if(flagTime){
    portENTER_CRITICAL(&timerMux);
    flagTimer = false;
    portEXIT_CRITICAL(&timerMux);
    countStatusLed++;
    if(countStatusLed > TIME_2Seg){
      countStatusLed = 0;
      Serial.print("LED nucleo: ");
      Serial.println(xPortGetCoreID());
    }
      
    if(flagConfig){
      if(countStatusLed == 0 || countStatusLed == 3 || countStatusLed == 6){
        digitalWrite(statusLED, HIGH);
      }else{
        digitalWrite(statusLED, LOW);
      }
    }else if(WiFi.isConnected()){
      if(countStatusLed == 0){
        digitalWrite(statusLED, HIGH);
      }else{
        digitalWrite(statusLED, LOW);
      }
    }else{
      if(countStatusLed == 0 || countStatusLed == 3){
        digitalWrite(statusLED, HIGH);
      }else{
        digitalWrite(statusLED, LOW);
      }
    }
    //verButton();

  }
}
const char* ssidap     = "AP_Home_IoT_";
const char* passwordAP = "123456789";
void modeConfig(){ 
  uint64_t chipid=ESP.getEfuseMac();
  uint16_t chip = (uint16_t)(chipid >> 32);
  String chipStrint = (String)chip;
  String ssidAP = ssidap + (String)chip;
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssidAP.c_str(), passwordAP);
  configServer();
}

void verButton(){
  portENTER_CRITICAL(&timerMux);
  bool flag = flagButton;
  portEXIT_CRITICAL(&timerMux);
  if(flag){
    portENTER_CRITICAL(&timerMux);
    flagButton = false;
    portEXIT_CRITICAL(&timerMux);
    if(!digitalRead(bottonProg)){
      if(flag1Vez){
        flag1Vez = false;
        digitalWrite(LED, !digitalRead(LED));
      }
      countProg++;
      if(countProg > TIME_4Seg){
        countProg = 0;
        digitalWrite(LED, !digitalRead(LED));
        flagConfig = !flagConfig;
        if(flagConfig){
          modeConfig();
          configServer();
        }
        #if DEBUG_MODE > 0
          Serial.println("\nToggle Modo Configuracion");
        #endif
      }
    }else{
      flag1Vez = true;
      countProg = 0;
      
    }
  }
}

void readEepromAllUser(void){
  readAllUser();
  if(cuentaUser.account[0] == 0xFFFFFFFF || cuentaUser.account[0] == 0x0){
    falgAdmin = false;
  }
  else{
    falgAdmin = true;
  }
}

void imprimirAllUser(void){
  //readAllUser();
  readEepromAllUser();
  Serial.println("Usuarios Registrados en la EEPROM:");
  for (size_t i = 0; i < USER_ALL; i++)
  {
    Serial.print("Usuario "); Serial.print(i); Serial.print(": ");
    if(cuentaUser.account[i] == 0xFFFFFFFF)
      Serial.println("Sin Usuasrio");
    else
      Serial.println(cuentaUser.account[i]);
  }
  
}

void verTelegram(void){
  portENTER_CRITICAL(&timerMux);
  bool flag = flagTelegram;
  portEXIT_CRITICAL(&timerMux);
  if(flag){
    portENTER_CRITICAL(&timerMux);
    flagTelegram = false;
    portEXIT_CRITICAL(&timerMux);
    countTelegram++;
    if(countTelegram > TIME_2Seg){
      countTelegram = 0;
      int requestGet = requestGetUpdateClear(tokenBot);
      if(requestGet == 200){
        Serial.print("ver Telegram nucleo: ");
        Serial.println(xPortGetCoreID());
        if(userMsg.messageType == MessageText){
          // #if DEBUG_MODE > 0
          //   Serial.println("\nRespuesta de requestGetUpdateClear Exitosa");
          //   Serial.print("Mensaje Tipo: "); Serial.println(userMsg.text);
          //   Serial.println(userMsg.user.id);
          //   Serial.println(userMsg.user.username);
          //   Serial.println(userMsg.user.firstName);
          //   Serial.println(userMsg.user.lastName);
          //   Serial.println(userMsg.text);
          // #endif
          if(userMsg.text == CommadSTART){
            #if DEBUG_MODE > 0
              Serial.println("\nSe Rx CommadSTART");
            #endif
            String msgStart = "Bienvenido al BOT que Automatiza el Porton.\n/registrar - Comando para Registrarse en el BOT.\nSu chat id es: ";
            msgStart += userMsg.user.id;
            msgStart += "\nBienvenido: ";
            msgStart += userMsg.user.username;
            int requestCommand = sendComand(userMsg.user.id, msgStart);
            if(requestCommand == 200){
              #if DEBUG_MODE > 0
                Serial.println("Respuesta a la Tx del BOT Exitosa");
              #endif
            }else{
              requestCommand = sendComand(userMsg.user.id, msgStart);
              if(requestCommand == 200){
                #if DEBUG_MODE > 0
                  Serial.println("Reenvio a la Tx del BOT Exitosa");
                #endif
              }else{
                #if DEBUG_MODE > 0
                  Serial.println("Reenvio a la Tx del BOT Fallida");
                #endif
              }
            }
          }else if(userMsg.text == CommadREGISTRO){
            if(userIsRegistred()){
              String cmdRegistro = "Usuario ya Registrado, puede acceder mediante:\n/menu - Comando para ver opciones en el BOT.\nSu chat id es: ";
              cmdRegistro += userMsg.user.id;
              cmdRegistro +="\nBienvenido: ";
              cmdRegistro += userMsg.user.username;
              int requestCommand = sendComand(userMsg.user.id, cmdRegistro);
              if(requestCommand == 200){
                #if DEBUG_MODE > 0
                  Serial.println("\nRespuesta a la Tx del BOT Exitosa");
                #endif
              }else{
                #if DEBUG_MODE > 0
                  Serial.println("\nReenviando Rspuesta");
                #endif
                requestCommand = sendComand(userMsg.user.id, cmdRegistro);
                if(requestCommand == 200){
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Exitosa");
                  #endif
                }else{
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Fallida");
                  #endif
                }
              }
            }else{
              readAllUser();
              if(cuentaUser.account[0] == 0xFFFFFFFF || cuentaUser.account[0] == 0x0){
                cuentaUser.account[0] = userMsg.user.id;
                writeUser_i(0);
                writeUserName_i(0, userMsg.user.username);
              }else{
                //bool statusReg = 
                if(verRegisterActive()){
                  enviarRegistrarTel();
                  guardarREgistrarTel();
                }else{
                  registoPendiente();
                }
                
              }
            }
          }else if(userMsg.text == CommadMENU){
            if(userIsRegistred()){
              int requestCommand = sendMenu(userMsg.user.id);
              if(requestCommand == 200){
                #if DEBUG_MODE > 0
                  Serial.println("\nRespuesta a la Tx del BOT Exitosa");
                #endif
              }else{
                #if DEBUG_MODE > 0
                  Serial.println("\nReenviando Rspuesta");
                #endif
                requestCommand = sendMenu(userMsg.user.id);
                if(requestCommand == 200){
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Exitosa");
                  #endif
                }else{
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Fallida");
                  #endif
                }
              }
            }else{
              userNoRegister();
            }
            
          // }
          // else if(userMsg.text == CommadOption){
          //   int requestCommand = sendOption(userMsg.user.id);
          }
          else if(userMsg.text == CommadLED_ON){
            if(userIsRegistred()){
              digitalWrite(LED, HIGH);
              String mensaje = "Usuario: ";
              mensaje += userMsg.user.username;
              mensaje += " LED en ON";
              int requestCommand = sendComand(userMsg.user.id, mensaje);
              if(requestCommand == 200){
                #if DEBUG_MODE > 0
                  Serial.println("\nRespuesta a la Tx del BOT Exitosa");
                #endif
              }else{
                #if DEBUG_MODE > 0
                  Serial.println("\nRespuesta a la Tx del BOT Fallida");
                #endif
              }
              mensaje = "El Usuario: ";
              mensaje += userMsg.user.username;
              mensaje += "\nActivo LED en ON por BOT";
              requestCommand = sendComand(cuentaUser.account[0], mensaje);
            }else{
              userNoRegister();
            }
          }
          else if(userMsg.text == CommadLED_OFF){
            if(userIsRegistred()){
              digitalWrite(LED, LOW);
              String mensaje = "Usuario: ";
              mensaje += userMsg.user.username;
              mensaje += " LED en OFF";
              int requestCommand = sendComand(userMsg.user.id, mensaje);
              if(requestCommand == 200){
                #if DEBUG_MODE > 0
                  Serial.println("\nRespuesta a la Tx del BOT Exitosa");
                #endif
              }else{
                #if DEBUG_MODE > 0
                  Serial.println("\nReenviando Rspuesta");
                #endif
                requestCommand = sendComand(userMsg.user.id, mensaje);
                if(requestCommand == 200){
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Exitosa");
                  #endif
                }else{
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Fallida");
                  #endif
                }
              }
              mensaje = "El Usuario: ";
              mensaje += userMsg.user.username;
              mensaje += "\nActivo LED en OFF por BOT";
              requestCommand = sendComand(cuentaUser.account[0], mensaje);
            }else{
              userNoRegister();
            }
          }
          else if(userMsg.text == CommadLED_Toggle){
            if(userIsRegistred()){
              digitalWrite(LED, !digitalRead(LED));
              String mensaje = "Usuario: ";
              mensaje += userMsg.user.username;
              if(digitalRead(LED))
                mensaje += " LED en ON";
              else
                mensaje += " LED en OFF";
              int requestCommand = sendComand(userMsg.user.id, mensaje);
              if(requestCommand == 200){
                #if DEBUG_MODE > 0
                  Serial.println("\nRespuesta a la Tx del BOT Exitosa");
                #endif
              }else{
                #if DEBUG_MODE > 0
                  Serial.println("\nReenviando Rspuesta");
                #endif
                requestCommand = sendComand(userMsg.user.id, mensaje);
                if(requestCommand == 200){
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Exitosa");
                  #endif
                }else{
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Fallida");
                  #endif
                }
              }
              mensaje = "El Usuario: ";
              mensaje += userMsg.user.username;
              if(digitalRead(LED))
                mensaje += "\nLED en ON por BOT";
              else
                mensaje += "\nLED en OFF por BOT";
              requestCommand = sendComand(cuentaUser.account[0], mensaje);
            }else{
              userNoRegister();
            }
          }
          else if(userMsg.text == CommadLED_Status){
            if(userIsRegistred()){
              String mensaje = "Usuario: ";
              mensaje += userMsg.user.username;
              if(digitalRead(LED))
                mensaje += " LED en ON";
              else
                mensaje += " LED en OFF";
              int requestCommand = sendComand(userMsg.user.id, mensaje);
              if(requestCommand == 200){
                #if DEBUG_MODE > 0
                  Serial.println("\nRespuesta a la Tx del BOT Exitosa");
                #endif
              }else{
                #if DEBUG_MODE > 0
                  Serial.println("\nReenviando Rspuesta");
                #endif
                requestCommand = sendComand(userMsg.user.id, mensaje);
                if(requestCommand == 200){
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Exitosa");
                  #endif
                }else{
                  #if DEBUG_MODE > 0
                    Serial.println("\nReenvio a la Tx del BOT Fallida");
                  #endif
                }
              }
              mensaje = "El Usuario: ";
              mensaje += userMsg.user.username;
              mensaje += "\nSolicito Estatus de LED por BOT";
              requestCommand = sendComand(cuentaUser.account[0], mensaje);
            }else{
              userNoRegister();
            }
          }
          else{
            String reply;
            if(userIsRegistred()){
              reply = "Usuario: " ;
              reply += userMsg.user.username;
              reply += ".\nYa usted esta Registrado";
              reply += ".\nComando: /menu";
              reply += ".\nPara opciones del sistema";
              int requestCommand = sendComand(userMsg.user.id, reply);
              if(requestCommand == 200){
                #if DEBUG_MODE > 0
                  Serial.println("Respuesta a la Tx del BOT Exitosa");
                #endif
              }else{
                #if DEBUG_MODE > 0
                  Serial.println("Respuesta a la Tx del BOT Fallida");
                #endif
              }
            }else{
              reply = "Bienvenido " ;
              reply += userMsg.user.username;
              reply += ".\nComando: /start";
              int requestCommand = sendComand(userMsg.user.id, reply);
              if(requestCommand == 200){
                #if DEBUG_MODE > 0
                  Serial.println("Respuesta a la Tx del BOT Exitosa");
                #endif
              }else{
                #if DEBUG_MODE > 0
                  Serial.println("Respuesta a la Tx del BOT Fallida");
                #endif
              }
            }
          }
        }
      }
        // else if (userMsg.messageType == MessageQuery){
        //   if(userMsg.callbackQueryData){
        //     //if(userMsg.callbackQueryData == queryON){
        //     if(strcmp(userMsg.callbackQueryData, queryON.c_str())==0){
        //       digitalWrite(LED, HIGH);
        //       //sendanswerCallbackQuery();
        //       String mensaje = "Usuario: ";
        //       mensaje += userMsg.user.username;
        //       mensaje += " LED en ON";
        //       int requestCommand = sendComand(userMsg.user.id, mensaje);
        //       if(requestCommand == 200){
        //         #if DEBUG_MODE > 0
        //           Serial.println("\nRespuesta a la Tx del BOT Exitosa");
        //         #endif
        //       }else{
        //         #if DEBUG_MODE > 0
        //           Serial.println("\nRespuesta a la Tx del BOT NO Realizada");
        //         #endif
        //       }
        //     //}else if(userMsg.callbackQueryData == queryOFF){
        //     }else if(strcmp(userMsg.callbackQueryData, queryOFF.c_str())==0){
        //       digitalWrite(LED, LOW);
        //       //sendanswerCallbackQuery();
        //       String mensaje = "Usuario: ";
        //       mensaje += userMsg.user.username;
        //       mensaje += " LED en OFF";
        //       int requestCommand = sendComand(userMsg.user.id, mensaje);
        //       if(requestCommand == 200){
        //         #if DEBUG_MODE > 0
        //           Serial.println("\nRespuesta a la Tx del BOT Exitosa");
        //         #endif
        //       }else{
        //         #if DEBUG_MODE > 0
        //           Serial.println("\nRespuesta a la Tx del BOT NO Realizada");
        //         #endif
        //       }
        //     //}else if(userMsg.callbackQueryData == queryToggle){
        //     }else if(strcmp(userMsg.callbackQueryData, queryToggle.c_str())==0){
        //       digitalWrite(LED, !digitalRead(LED));
        //       //sendanswerCallbackQuery();
        //       String mensaje = "Usuario: ";
        //       mensaje += userMsg.user.username;
        //       if(digitalRead(LED))
        //         mensaje += " LED en ON";
        //       else
        //         mensaje += " LED en OFF";
        //       int requestCommand = sendComand(userMsg.user.id, mensaje);
        //       if(requestCommand == 200){
        //         #if DEBUG_MODE > 0
        //           Serial.println("\nRespuesta a la Tx del BOT Exitosa");
        //         #endif
        //       }else{
        //         #if DEBUG_MODE > 0
        //           Serial.println("\nRespuesta a la Tx del BOT NO Realizada");
        //         #endif
        //       }
        //     //}else if(userMsg.callbackQueryData == queryStatus){
        //     }else if(strcmp(userMsg.callbackQueryData, queryStatus.c_str())==0){
        //       //sendanswerCallbackQuery();
        //       String mensaje = "Usuario: ";
        //       mensaje += userMsg.user.username;
        //       if(digitalRead(LED))
        //         mensaje += " LED en ON";
        //       else
        //         mensaje += " LED en OFF";
        //       int requestCommand = sendComand(userMsg.user.id, mensaje);
        //       if(requestCommand == 200){
        //         #if DEBUG_MODE > 0
        //           Serial.println("\nRespuesta a la Tx del BOT Exitosa");
        //         #endif
        //       }else{
        //         #if DEBUG_MODE > 0
        //           Serial.println("\nRespuesta a la Tx del BOT NO Realizada");
        //         #endif
        //       }
        //     }
        //   }
        // }

    }
  }
}

bool userIsRegistred(void){
 bool  resp = false;
 for (uint8_t i = 0; i < USER_ALL; i++)
 {
   if(userMsg.user.id == cuentaUser.account[i]){
     resp = true;
     break;
   }
 }
 return (resp);
}

void userNoRegister(void){
  String msgReg = "Usuario no Registrado envíe:.\n/registrar - Para Registrarse en el BOT.\nSu chat id es: ";
  msgReg += userMsg.user.id;
  msgReg += "\nNombre Usuario: ";
  msgReg += userMsg.user.username;
  int requestCommand = sendComand(userMsg.user.id, msgReg);
  if(requestCommand == 200){
    #if DEBUG_MODE > 0
      Serial.println("\nEnvío a la Tx del BOT Exitosa");
    #endif
  }else{
    #if DEBUG_MODE > 0
      Serial.println("\nEnvio de comando Fallida");
    #endif
  }
}

void enviarRegistrarTel(void){
  String cmdoRegistro = "Su Registro fue enviado al Administrador.\nCuando este Responda se Actualizara su Estatus.";
  cmdoRegistro +="\nUsuario: ";
  cmdoRegistro += userMsg.user.username;
  cmdoRegistro += "\nSu chat id es: ";
  cmdoRegistro += userMsg.user.id;
  
  int requestCommand = sendComand(userMsg.user.id, cmdoRegistro);
  if(requestCommand == 200){
    #if DEBUG_MODE > 0
      Serial.println("enviarRegistrarTel BOT Exitosa");
    #endif
  }else{
    #if DEBUG_MODE > 0
      Serial.println("enviarRegistrarTel BOT Fallida");
    #endif
  }
  cmdoRegistro = "";
  cmdoRegistro = "Administrador, el siguiente:.\n";
  cmdoRegistro += "Usuari@: ";
  cmdoRegistro += userMsg.user.username;
  cmdoRegistro += "\nCon ID: ";
  cmdoRegistro += userMsg.user.id;
  cmdoRegistro += "Realizó una Solicitud de registro en el sistema";
  requestCommand = sendComand(cuentaUser.account[0], cmdoRegistro);
  if(requestCommand == 200){
    #if DEBUG_MODE > 0
      Serial.println("enviarRegistrarTel BOT Exitosa");
    #endif
  }else{
    #if DEBUG_MODE > 0
      Serial.println("enviarRegistrarTel BOT Fallida");
    #endif
  }
}

void limpiarRegUser(void){
  Serial.println("Clear Registros");
  for (uint8_t i = 0; i < USER_ALL; i++)
  {
    regUserStore[i].userIdReg = 0;
    regUserStore[i].userNameReg = "";
  }
}

void guardarREgistrarTel(void){
  Serial.println("Almac. Reg");
  for (uint8_t i = 0; i < USER_ALL; i++)
  {
    if (regUserStore[i].userIdReg == 0)
    {
      regUserStore[i].userIdReg = userMsg.user.id;
      regUserStore[i].userNameReg = userMsg.user.username;
      break;
    }
  }
}
//Retorna TRUE si no Tiene Registro Pendiente
bool verRegisterActive(void){
  //readAllUserRegPen();
  Serial.println("\nReg Pendiente???");
  // for (uint8_t i = 0; i < USER_ALL; i++){
  //   Serial.print("User Name "); Serial.print(i); Serial.print(": "); Serial.println(regUserStore[i].userNameReg);
  //   Serial.print("User ID "); Serial.print(i); Serial.print(": "); Serial.println(regUserStore[i].userIdReg);
  // }
  bool resp = true;
  for (uint8_t i = 0; i < USER_ALL; i++){
    if(regUserStore[i].userIdReg == userMsg.user.id){
      resp = false;
      Serial.println("Hay Registro Pendiente");
      break;
    }
  }
  return (resp);
}

void registoPendiente(void){
   String cmdoRegistro = "Ya posee un Registro pendiente.\nPongase en contacto con el Administrador para que responda su solicitud.";
  cmdoRegistro +="\nNombre de Usuario: ";
  cmdoRegistro += userMsg.user.username;
  cmdoRegistro += "\nSu chat id es: ";
  cmdoRegistro += userMsg.user.id;
  
  int requestCommand = sendComand(userMsg.user.id, cmdoRegistro);
  if(requestCommand == 200){
    #if DEBUG_MODE > 0
      Serial.println("enviarRegistrarTel BOT Exitosa");
    #endif
  }else{
    #if DEBUG_MODE > 0
      Serial.println("enviarRegistrarTel BOT Fallida");
    #endif
  }
}

bool adminRegistra(void){
  bool resp = false;
  readAllUser();
  for (uint8_t i = 0; i < USER_ALL; i++)
  {
    if(cuentaUser.account[i] == 0xFFFFFFFF || cuentaUser.account[i] == 0x0){
      cuentaUser.account[i] = admidId;
      writeUser_i(i);
      writeUserName_i(i, adminUser);
      resp = true;
      break;
    }
  }
  return resp;
}

bool adminRegistraUser(void){
  bool resp = false;
  readAllUser();
  for (uint8_t i = 0; i < USER_ALL; i++)
  {
    if(cuentaUser.account[i] == 0xFFFFFFFF || cuentaUser.account[i] == 0x0){
      cuentaUser.account[i] = userID;
      writeUser_i(i);
      writeUserName_i(i, userName);
      resp = true;
      break;
    }
  }
  return resp;
}

void enviarRegPend(void){
  String jsonString;
  StaticJsonDocument<2048> root;
  root.clear();
  DynamicJsonDocument doc(1500);
  doc.clear();
  JsonArray arrayWifi = doc.to<JsonArray>();
  JsonObject s1 = arrayWifi.createNestedObject();
  s1.clear();
  root["error"] = 0;
  uint8_t countReg = 0;
  for (int i = 0; i < USER_ALL; ++i){
    if (regUserStore[i].userIdReg != 0){
      countReg++;
    }
  }
  if(countReg == 0){
    //arrayWifi.add();
  }else{
    for (int i = 0; i < countReg; ++i){
      s1["userId"] = regUserStore[i].userIdReg;
      s1["userName"] = regUserStore[i].userNameReg;
      arrayWifi.add(s1);
    }
  }
  arrayWifi.remove(0);
  root["data"] = arrayWifi;
  serializeJson(root, jsonString);
  Serial.println("\nRegistros Pendientes:");
  Serial.println(jsonString);
  sendRequest(jsonString);
  limpiarRegUser();
}

void enviarUserRequest(void){
  readAllUserName();
  readAllUser();
  String jsonString;
  StaticJsonDocument<2048> root;
  root.clear();
  DynamicJsonDocument doc(1500);
  doc.clear();
  JsonArray arrayWifi = doc.to<JsonArray>();
  JsonObject s1 = arrayWifi.createNestedObject();
  s1.clear();
  root["error"] = 0;

  for (int i = 0; i < USER_ALL; ++i){
    if(cuentaUser.account[i] != 0xFFFFFFFF && cuentaUser.account[i] != 0x0){
      s1["regID"] = i;
      s1["userId"] = cuentaUser.account[i];
      s1["userName"] = cuentaUserName.account[i];
      arrayWifi.add(s1);
    }
  }
  arrayWifi.remove(0);
  root["data"] = arrayWifi;
  serializeJson(root, jsonString);
  Serial.println("\nVer All User:");
  Serial.println(jsonString);
  sendRequest(jsonString);
}

bool verCuenta(int32_t scanUserId){
  bool resp = false;
  readAllUser();
  for(int i = 0; i < USER_ALL; ++i){
    if(scanUserId == cuentaUser.account[i]){
      resp = true;
      break;
    }
  }
  return resp;
}

void envRegistroUser(void){
  String msgStart = "Usuario: ";
  msgStart += userName;
  msgStart += "ID de Usuario: ";
  msgStart += userID;
  msgStart += "\nSu solicitud fue aceptada y registrada por le Administrador.";
  msgStart += "\nEnvíe el comando:";
  msgStart += "\n/menu -> para ver las opciones del Sistema";
  int requestCommand = sendComand(userID, msgStart);
  if(requestCommand == 200){
    #if DEBUG_MODE > 0
      Serial.println("Respuesta a la Tx del BOT Exitosa");
    #endif
  }
}
