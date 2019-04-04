#ifndef RWS_WIFI_H
#define RWS_WIFI_H

#include <ESP8266WiFiMulti.h>

/**
 * @brief description of the rws_wifi object
 *
 * It uses the ESP8266WiFiMulti object to manage the
 * connection.
 *
 */
class rws_wifi
{
    public:
        rws_wifi();
        ~rws_wifi();

        void addAP(const char* ssid, const char *passphrase = NULL);
        void check_connection(void);
        bool connected(void);

    private:
        WifiAPlist APlist;
        ESP8266WiFiMulti wifiMulti;

        void APlistClean(void);
};

#endif // RWS_WIFI_H