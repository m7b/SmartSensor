# Automated watering system

### Test results

Branches  | Linux | Windows | Test coverage |
----------|-------|---------|---------------|
Master:   | [![Build Status](https://travis-ci.org/m7b/SmartSensor.svg?branch=master)](https://travis-ci.org/m7b/SmartSensor) | [![Build status](https://ci.appveyor.com/api/projects/status/f4np60o1tfdux3ha?svg=true)](https://ci.appveyor.com/project/m7b/smartsensor) |   |


### About

IoT project for an automated watering system with serval ESP8266 for water level sensors and controller.

### Build instructions

Developed with [Visual Studio Code](https://code.visualstudio.com) and [PlatformIO](https://platformio.org).

### MQTT Topics

 - WS/RWS/EG/BarrelSrc/Lvl/sensorcm
 - WS/RWS/EG/BarrelSrc/Lvl/cm
 - WS/RWS/EG/BarrelSrc/Lvl/percent
 - WS/RWS/EG/BarrelSrc/Lvl/timestamp
 - WS/RWS/EG/BarrelSrc/Lvl/local_timestamp

 - WS/RWS/EG/BarrelSrc/FunctionModeReq
 - WS/RWS/EG/BarrelSrc/FunctionModeAck

 - WS/RWS/EG/BarrelSrc/Status (also mqtt last will topic)
 
 WS = WateringSystem
 RWS = RainWaterSystem
 
Controller