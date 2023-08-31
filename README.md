# ESP-Temp-Sensor-Portal

WiFi термометр на ESP-01/ESP-01s.

### Возможности: 
- Управление через web портал;
- Управление через MQTT;
- Интеграция с [HomeAssistant](https://www.home-assistant.io) через MQTT;
- Обновление прошивки через web портал;
- Переход в режим точки доступа, при недоступности Wi-Fi указанного в настройках;
- Переход в режим клиента Wi-Fi, при доступности WiFi указанного в настройках.


### Интерфейс web портала:
<img width="374" alt="image" src="https://github.com/mr-whitefoot/ESP-Temp-sensor-DS1820B-Portal/assets/16363451/a8b08b90-9e7b-4221-87ef-3d2ca3ec5cba">

Выбор настроек<br>
<img width="401" alt="image" src="https://github.com/mr-whitefoot/ESP-Relay-Portal/assets/16363451/f3f8e971-b77e-4998-bc1a-49f401978e52">

Обновление прошивки по Wi-Fi<br>
<img src="https://user-images.githubusercontent.com/16363451/197058992-d8bc1296-aa61-4ff9-ba36-1ad8a007244e.png" width="400">


### Первоначальная настройка:
- Подключиться к точке доступа relayAP;
- Откроется портал, если не откроется, перейти по адресу 192.168.4.1;
- Нажать кнопку Configuration;
- Нажать WiFi configuration;
- Указать данные вашего Wi-Fi: SSID, пароль;
- Нажать кнопку Save.
- При успешном соединении статус Wi-Fi сменится на зеленый, так же отобразится уровень сигнала и IP адрес. По истечению 3х минут точка доступа будет деактивирована. 
- Подключитесь к вашей точке доступа и перейдите на IP адрес из пункта выше. 


### Обновление прошивки через портал:
- Скачать [последнюю версию прошивки](https://github.com/mr-whitefoot/ESP-Temp-sensor-DS1820B-Portal/releases/latest);
- Перейти на портале устройства Configuration -> Firmware upgrade;
- Нажать кнопку OTA firmware;
- Выбрать скачанный bin файл;
- Дождаться обновления страницы с сообщением Update Success! Rebooting...;
- Подождать не менее 25 секунд и нажать кнопку Home, для перехода на главную страницу;
- Проверить версию устройства в блоке Information.


