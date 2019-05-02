# Current Consumption Sensor

## Ultrasonic sensor RCW-0001
[Datasheet](https://www.cytron.io/p-rcw-0001-micro-ultrasonic-range)
Operating current (3.3V): 2.2mA

## ESP-01
[Table example values](https://www.dinotools.de/2015/07/14/esp8266-01-power-consumption)


| Mode              | Current consumption |
|-------------------|---------------------|
| Normal operation  | 140 - 160 mA        |
| Deep sleep        |                     |


# Mosquitto on Synology NAS

https://www.msxfaq.de/sonst/iot/mqtt.htm
https://synocommunity.com/package/mosquitto
https://community.home-assistant.io/t/mosquitto-systemctl-not-starting/8978/7
https://github.com/SynoCommunity/spksrc/issues/2581


/usr/local/mosquitto/var/mosquitto.conf

# Mosquitto Server logging

[Understanding and Configuring Logging – Mosquitto Broker Configuration](http://www.steves-internet-guide.com/mosquitto-logging/)

# Sensor behaviour

- Sensor switch on
[SETUP]
- Try WLAN connection for 10 sec
  - if no connection could be established, go to deep sleep for 5 min
- Try MQTT connections for 5 times
  - if no connection could be established, go to deep sleep for 5 min
[LOOP]
do same things as is SETUP