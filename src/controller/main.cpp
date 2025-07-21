#include "controller/controller.h"

//Set dst/std rules
TimeChangeRule rCEST = {CEST_ABBREV, CEST_WEEK, CEST_DOW, CEST_MONTH, CEST_HOUR, CEST_OFFSET};
TimeChangeRule rCET  = {CET_ABBREV,  CET_WEEK,  CET_DOW,  CET_MONTH,  CET_HOUR,  CET_OFFSET};
Timezone tz(rCEST, rCET);

rws_ntp ntp(NTP_SERVER, NTP_OFFSET_S, NTP_UPDATE_INTERVAL_MS, &tz);
rws_syslog syslog(SYSLOG_SERVER, SYSLOG_PORT, DEVICE_HOSTNAME, APP_NAME, LOG_KERN);
rws_mqttclient mqtt(MQTT_SERVER, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS, LAST_WILL_TOPIC, LAST_WILL_QOS, LAST_WILL_RETAIN, LAST_WILL_MESSAGE);

//Web-Updater things---------------------
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
rws_webupdate webUpdate(MQTT_CLIENT_ID, &httpServer, &httpUpdater, MQTT_CLIENT_ID);
//Web-Updater things---------------------

controller ctrl(&WiFi, &ntp, &syslog, &mqtt, &webUpdate);

/**
 * @brief setup section of board
 * 
 */
void setup(void) {
    //Setup the controller
    ctrl.setup();

    //Init random numberings    
    randomSeed(micros());
}

/**
 * @brief loop section of the board
 * 
 */
void loop(void) {
    //Perform the controller
    ctrl.loop();
}

/* #################################################### */

time_t _MJBgetNtpTime(void)
{
    return ntp.get_local_datetime_t();
}