# Automated watering system

### Test results

Branches  | Linux | Windows | Test coverage | Security analysis |
----------|-------|---------|---------------|-------------------|
Master:   | [![Build Status](https://travis-ci.com/m7b/SmartSensor.svg?branch=master)](https://travis-ci.com/m7b/SmartSensor) | [![Build status](https://ci.appveyor.com/api/projects/status/f4np60o1tfdux3ha?svg=true)](https://ci.appveyor.com/project/m7b/smartsensor) |   | [![Total alerts](https://img.shields.io/lgtm/alerts/g/m7b/SmartSensor.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/m7b/SmartSensor/alerts/)


### About

IoT project for an automated watering system with serval ESP8266 for water level sensors and controller.

### Build instructions

Developed with [Visual Studio Code](https://code.visualstudio.com) and [PlatformIO](https://platformio.org).

### MQTT Topics

 - WS/RWS/EG/BarrelSrc/Lvl/sensorcm
 - WS/RWS/EG/BarrelSrc/Lvl/cm
 - WS/RWS/EG/BarrelSrc/Lvl/percent
 - WS/RWS/EG/BarrelSrc/Lvl/litres
 - WS/RWS/EG/BarrelSrc/Lvl/timestamp
 - WS/RWS/EG/BarrelSrc/Lvl/local_timestamp

 - WS/RWS/EG/BarrelSrc/FunctionModeReq
 - WS/RWS/EG/BarrelSrc/FunctionModeAck

 - WS/RWS/EG/BarrelSrc/Status (also mqtt last will topic)
 
 WS = WateringSystem
 RWS = RainWaterSystem
 
Controller

### Setup mosquitto hints

- finding mosquitto executables: `sudo find / -name "mosquitto*"`
- finding running instance with path and config file: `sudo ps -aux | grep mosquitto`
- [General Command Line commands for handling DSM Services](https://tech.setepontos.com/2018/03/25/control-synology-dsm-services-via-terminal-ssh/)
- [Configuring Username and Password Authentication on Mosquitto MQTT Broker](https://www.youtube.com/watch?v=IenXQvOcj54).
- [How to Configure SSL on the Mosquitto MQTT Broker](https://www.youtube.com/watch?v=f3f4h7q6x5g).
- [How to Install and Secure the Mosquitto MQTT Messaging Broker](https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-ubuntu-18-04-quickstart)
- [username and password] (http://www.steves-internet-guide.com/mqtt-username-password-example/)


