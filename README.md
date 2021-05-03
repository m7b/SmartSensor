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
- [Configuring Username and Password Authentication on Mosquitto MQTT Broker](https://www.youtube.com/watch?v=IenXQvOcj54).
- [How to Configure SSL on the Mosquitto MQTT Broker](https://www.youtube.com/watch?v=f3f4h7q6x5g).


