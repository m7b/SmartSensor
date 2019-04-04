#include "rws_wifi.h"

/**
 * @brief Construct a new smartsensor wifi::smartsensor wifi object
 *
 * Automatic add some ssid and passwords.
 *
 */
rws_wifi::rws_wifi()
{
}

/**
 * @brief Destroy the smartsensor wifi::smartsensor wifi object
 *
 * Clears the list or ssid and its passwords
 *
 */
rws_wifi::~rws_wifi()
{
    APlistClean();
}

/**
 * @brief check and wait for connection
 *
 */
void rws_wifi::check_connection(void)
{
    if (connected())
        return;

    Serial.println("");
    Serial.println("Connecting Wifi to one of following SSIDs ...");

    //Print registred SSIDs to try to connect
    for(auto entry : APlist) {
        Serial.print(" - ");
        Serial.println(entry.ssid);
    }

    while (wifiMulti.run() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("SSID: ");
    Serial.print(WiFi.SSID());
    Serial.print(", IP address: ");
    Serial.println(WiFi.localIP());
}

/**
 * @brief Is WiFi connected
 *
 * @return true
 * @return false
 */
bool rws_wifi::connected(void)
{
    return WiFi.isConnected();
}

/**
 * @brief Adds ssid and passwords to a list
 *
 * @param ssid
 * @param passphrase
 */
void rws_wifi::addAP(const char* ssid, const char *passphrase)
{
    bool rc;

    rc = wifiMulti.addAP(ssid, passphrase);
    if (rc) {
        WifiAPEntry newAP;
        newAP.ssid = strdup(ssid);
        newAP.passphrase = strdup(passphrase);
        APlist.push_back(newAP);
    }
}

/**
 * @brief Clears the ssid and password list
 *
 */
void rws_wifi::APlistClean(void)
{
    for(auto entry : APlist) {
        if(entry.ssid) {
            free(entry.ssid);
        }
        if(entry.passphrase) {
            free(entry.passphrase);
        }
    }
    APlist.clear();
}