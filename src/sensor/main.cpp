#include <Arduino.h>
#include <rws_wifi.h>
#include <rws_ntp.h>
#include <rws_syslog.h>
#include <rws_pubsubclient.h>

#include "sensor/sensor.h"
#include "settings/smartsensor_settings.h"


//Set dst/std rules
TimeChangeRule rCEST = {CEST_ABBREV, CEST_WEEK, CEST_DOW, CEST_MONTH, CEST_HOUR, CEST_OFFSET};
TimeChangeRule rCET  = {CET_ABBREV,  CET_WEEK,  CET_DOW,  CET_MONTH,  CET_HOUR,  CET_OFFSET};
Timezone tz(rCEST, rCET);

rws_wifi wifiMulti;
rws_ntp ntp(NTP_SERVER, NTP_OFFSET_S, NTP_UPDATE_INTERVAL_MS, &tz);
rws_syslog syslog(SYSLOG_SERVER, SYSLOG_PORT, DEVICE_HOSTNAME, APP_NAME, LOG_KERN);
rws_pubsubclient mqtt(MQTT_SERVER, MQTT_PORT);


sensor sens(&wifiMulti, &ntp, &syslog, &mqtt);

//Initial operation mode
int OperationMode = INTERVAL_MEASURE__5_SEK;


/**
 * @brief setup section of board
 * 
 */
void setup(void) {
    //Setup the controller
    sens.setup();

    //Init random numberings    
    randomSeed(micros());
}

/**
 * @brief loop section of the board
 * 
 */
void loop(void) {
    //Perform the controller
    sens.loop();
}