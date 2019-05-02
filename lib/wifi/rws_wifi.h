#ifndef RWS_WIFI_H
#define RWS_WIFI_H

#include <ESP8266WiFiMulti.h>


#define ON_CON_TIMEOUT_FCT_SIGNATURE std::function<void(void)> on_connect_timeout_fct

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

        void set_on_con_timeout_fct(ON_CON_TIMEOUT_FCT_SIGNATURE);
        void addAP(const char* ssid, const char *passphrase = NULL);
        void check_connection(void);
        bool connected(void);

    private:
        WifiAPlist APlist;
        ESP8266WiFiMulti wifiMulti;

        void APlistClean(void);
        ON_CON_TIMEOUT_FCT_SIGNATURE;
};

#endif // RWS_WIFI_H