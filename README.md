# ESP32_Porton_Telegram
*Realizado por*:** APateti**
Proyecto para Controlar un Portón Electrico con ESP32 utilizando el servicio Telegram.

El control puede ser realizado mediante un BOT de Telegram o via WEB mediante API Json mediante una App (por ahora en Android).

Para Configurar inicialmente el Equipo, debe crearse un BOT y el primer usuario sera considerado como el Administrador del Sistema. Luego mediante la App ese administrador debe configurar el Equipo para que se conecte al Router que le dara acceso a Internet, para luego enviarle el Token del Bot al Equipo. Luego de tener la configuracion inicial, el administrador debe suministrar a cada usuario que desee agregar al sistema el enlace del BOT, para que cada uno se registre de manera Individual. Hay que indicar que solo el Administrador puede permitir, o no, el registro de cada usuario, y esto se hace mediante la App.
El Hardware utilizado es:<br>
[![Circuito ESP32-Telegram](Circuito "Circuito ESP32-Telegram")](https://user-images.githubusercontent.com/50499248/113419461-7c509c80-9395-11eb-9cd4-4d92ae791c15.jpg "Circuito ESP32-Telegram")

API del ESP32 controlador del Porton:
> **Comando: Toggle LED**<br>
Se Tx:<br>
`{ "cmdo" : "toggle", "data" : { "userName" : "Nombre del Usuario" "userID" : 123456789 } }`<br>
Se Rx<br>
Si Usuario Valido: <br>
`{ "error" : 0, "LED" : "ON" ó "OFF", } Si Usuario Invalido { "error" : 401, }`<br>

>**Comando  Acces Piont (Para Buscar Redes cercanas):**<br>
Se Tx:<br>
`{ "cmdo" : "accPoint", } `<br>
Se Rx:<br> 
`{ "error": 0, "wifi": [ { "ssid": "AP_Home_IoT", "qos": 4, "encryption": 3 }, { "ssid": "MERCUSYS_7A5B", "qos": 2, "encryption": 4 }, { "ssid": "AP_Home_IoT_R", "qos": 2, "encryption": 3 }, { "ssid": "AP_MQTT_PIR_11E15C", "qos": 1, "encryption": 3 } ] } `

> **Comando Conectarse Acces Piont (Se conecta al Router Seleccionado):**<br>
Se Tx:<br>
`{ "cmdo" : "conectAP", "data" : { "ssid" : "SSID Router" "pass" : "PASS Router" } }`<br>
Se Rx <br>
si se conecta: <br>
`{ "error" : 0, "data" : { "dirIP" : "192.168.1.x" } }`<br>
si NO se conecta: <br>
`{ "error" : 1, "data" : { "dirIP" : "" } }`<br>

> **Comando Configurar Token del Bot (Se envia el Token del BOT):**<br>
Se Tx:<br>
`{ "cmdo" : "tokenBot", "data" : { "token" : "1163113539:AAEZfgENIoMnjdzN3kdBrqZiCGMjjC_ZcdU" } } Rx si Token valido { "error" : 0, }`<br>
Se Rx si Token no se Conecta<br>
`{ "error" : 401, }`<br>

> **Comando Comando para BORRAR toda la Configuración (Se borra toda la EEPROM):**<br>
Se Tx:<br>
`{ "cmdo" : "erraseEE", "data" : { "userName" : "Nombre del Usuario" "userID" : 123456789 } }`<br>
Rx Si es por el Administrador <br>
`{ "error" : 0, }`<br>
Se Rx Si NO es por el Administrador <br>
`{ "error" : 401, } `<br>

>**Tx Comando para Ver Todos los Usuarios (Se Rex en Json todos los Usuarios Registrados):**<br>
Se Tx si es Administrador:<br>
`{ "cmdo" : " verAllUser", "data" : { "userName" : "Nombre del Usuario" "userID" : 123456789 } } `<br>
Rx Si es por el Administrador <br>
`{ "error": 0, "data": [ { "regID": 0, "userId": 719665380, "userName": "" }, { "regID": 1, "userId": 823052773, "userName": "DPateti" } ] }`<br>
Rx Si NO es por el Administrador<br>
`{ "error" : 401, }`<br>

> **Tx Comando para Registrar Usuarios (Se Registra un Usuario almacenándolo en la EEPROM):** <br>
Se Tx:<br>
`{ "cmdo" : " registrar", "data" : { "userName" : "Nombre del Usuario" "userID" : 123456789, "adminName" : "Nombre del Administrador" "adminID" : 123456789}}`<br>
Rx Si es por el Administrador y se Graba en la EEPROM 
`{ "error": 0, }`<br>
Rx Si es por el Administrador y NO se puede Graba en la EEPROM<br>
`{ "error" : 400, }`<br>
Rx Si NO es por el Administrador<br>
`{ "error" : 401, }`<br>

> **Tx Comando para Ver Todos los Usuarios que solicitaron Registrarse (Se Rx en Json todos los Usuarios que solicitaron Registrados):**<br>
Se Tx:<br>
`{ "cmdo" : " verRegistro", "data" : { "userName" : "Nombre del Usuario" "userID" : 123456789 } }`<br>
Rx Si es por el Administrador<br>
`{ "error": 0, "data": [ { "regID": 0, "userId": 719665380, "userName": "" }, { "regID": 1, "userId": 823052773, "userName": "DPateti" } ] }`<br>
Rx Si NO es por el Administrador<br>
`{ "error" : 401, }`<br>

> **Tx Comando para Eliminar un Usuarios (Se borra de la EEPROM el usuario):<br>
Se Tx:**<br>
`{ "cmdo" : " erraseUser", "data" : { "adminName" : "Nombre del Administrador" "adminID" : 123456789 "userName" : "Nombre del Usuario" "userID" : 123456789 "regID": 1, } }`<br>
Rx Si es por el Administrador y se borra el Usuario<br>
`{ "error": 0, }`<br>
Rx Si es por el Administrador y NO se borra el Usuario<br>
`{ "error" : 400, }`<br>
Rx Si NO es por el Administrador<br>
`{ "error" : 401, }`<br>
