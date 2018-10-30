#include <Arduino.h>
#include "smartsensor_wifi.h"
#include "smartsensor_ntp.h"
#include "smartsensor_syslog.h"
#include "smartsensor_pubsubclient.h"
#include "smartsensor_barrel.h"
#include "settings/smartsensor_settings.h"

smartsensor_wifi wifiMulti;
smartsensor_ntp ntp;
smartsensor_syslog syslog;
smartsensor_pubsubclient mqtt;

smartsensor_barrel barrel(&ntp, &mqtt, &syslog);

/**
 * @brief Setup the serial object
 * 
 */
void setup_serial(void) {
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

#if DEBUG == ON
    // Take time for opening serial window
    delay(10000);
#endif
}

/**
 * @brief Setup the wifi object
 * 
 */
void setup_wifi(void) {
    // We start by connecting to a WiFi network
    wifiMulti.check_connection();
}

/**
 * @brief Setup the ntp object
 * 
 */
void setup_ntp(void) {
    // We start getting time from ntp
    ntp.begin();
}

/**
 * @brief Setup the mqtt object
 * 
 */
void setup_mqtt(void) {
    // We start by connecting to MQTT server
    mqtt.check_connection();
}

/**
 * @brief Check all conditions ok
 * 
 * @return true 
 * @return false 
 */
bool check_all_conditions(void) {
    bool rc = true;
    
    rc = rc & wifiMulti.connected();
    rc = rc & mqtt.connected();
    rc = rc & ntp.update();

    return rc;
}

/**
 * @brief Setup the arduino programm
 * 
 * put your setup code here, to run once.
 */
void setup(void) {
    // Set up serial interface
    setup_serial();

    // Setup Wifi
    setup_wifi();

    // Setup NTP
    setup_ntp();

    // Setup MQTT
    setup_mqtt();
    
    randomSeed(micros());
}

/**
 * @brief Loop through the arduino programm
 * 
 * put your main code here, to run repeatedly.
 */
void loop(void) {
    //update time from NTP on demand, see NTP_UPDATE_INTERVAL_MS
    ntp.update();

    //check and renew WiFi connection
    wifiMulti.check_connection();

    //check and renew MQTT connection
    mqtt.check_connection();

    //check all conditions are ok
    if (!check_all_conditions())
        return;
    
    //Measure fill level
    barrel.do_measure();

    //Publish fill level
    barrel.do_publish();

    //Wait 5sec
    delay(5000);
}