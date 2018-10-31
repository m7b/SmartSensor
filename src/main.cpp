#include <Arduino.h>
#include "smartsensor_wifi.h"
#include "smartsensor_ntp.h"
#include "smartsensor_syslog.h"
#include "smartsensor_pubsubclient.h"
#include "smartsensor_barrel.h"
#include "smartsensor_statemachine.h"
#include "settings/smartsensor_settings.h"

smartsensor_wifi wifiMulti;
smartsensor_ntp ntp;
smartsensor_syslog syslog;
smartsensor_pubsubclient mqtt;

smartsensor_barrel barrel(&ntp, &mqtt, &syslog);

int OperationMode = 1;

void test_fct_callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println();

    if (strcmp(topic, mqtt.add_root_topic(FUNCTION_MODE).c_str()) == 0)
    {
        Serial.printf("Function mode received: %d\r\n", payload[0]);
        OperationMode = payload[0];
    }

    if (strcmp(topic, "inTopic") == 0)
    {
        Serial.printf("inTopic received: ");
        for (unsigned int i=0; i<length; i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();
    }
}

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
    //Set callback function
    mqtt.setCallback(test_fct_callback);

    // We start by connecting to MQTT server
    mqtt.check_connection();

    // Subscribe function mode
    mqtt.subscribe(mqtt.add_root_topic(FUNCTION_MODE).c_str());
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

    //keep mqtt alive
    mqtt.loop();
    
    //Measure fill level
    barrel.do_measure();

    //Publish fill level
    barrel.do_publish();

    switch (OperationMode)
    {
        case PERMANENT_MEASSURE:
            delay(200);
            break;

        case INTERVAL_MEASURE__5_SEK:
            delay(5000);
            break;

        case INTERVAL_MEASURE_10_SEK:
            delay(10000);
            break;

        case INTERVAL_MEASURE__5_MIN:
            delay(3600000);
            break;

        default:
            delay(7500);
            break;
    }
}