#ifndef SMARTSENSOR_WIFI_H
#define SMARTSENSOR_WIFI_H

#include <ESP8266WiFiMulti.h>

/**
 * @brief description of the smartsensor_wifi object
 * 
 * It uses the ESP8266WiFiMulti object to manage the
 * connection.
 * 
 */
class smartsensor_wifi
{
    public:
        smartsensor_wifi();
        ~smartsensor_wifi();
        
        void check_connection(void);
        bool connected(void);

    private:
        WifiAPlist APlist;
        ESP8266WiFiMulti wifiMulti;

        void addAP(const char* ssid, const char *passphrase = NULL);
        void APlistClean(void);
};

#endif // SMARTSENSOR_WIFI_H