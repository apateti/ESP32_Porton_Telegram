# ESP32_Porton_Telegram
Proyecto para Controlar un Portón Electrico con ESP32 utilizando el servicio Telegram.

El control puede ser realizado mediante un BOT de Telegram o via WEB mediante API Json mediante una App (por ahora en Android).

Para Configurar inicialmente el Equipo, debe crearse un BOT y elprimer usuario sera considerado como el Administrador del Sistema.
Luego mediante la App ese administrador debe configurar el Equipo para que se conecte al Router que le dara acceso a Internet, para luego enviarle el Token del Bot al Equipo.
Luego de tener la configuracion inicial, el administrador debe suministrar a cada usuari que desee agregar al sistema el enlace del BOT para que cada uno se registre de manera Individual. Hay que indicar que solo el Administrador puede permitir o no el registro de cada usuario, y esto se hace mediante la App
