#include "smartsensor_wifi.h"
#include "settings/wifi_access_datas.h"

/**
 * @brief Construct a new smartsensor wifi::smartsensor wifi object
 * 
 * Automatic add some ssid and passwords.
 * 
 */
smartsensor_wifi::smartsensor_wifi()
{
    WiFi.mode(WIFI_STA);

    for(auto entry : wifi_access_credentials) 
        addAP(entry.first, entry.second);
}

/**
 * @brief Destroy the smartsensor wifi::smartsensor wifi object
 * 
 * Clears the list or ssid and its passwords
 * 
 */
smartsensor_wifi::~smartsensor_wifi()
{
    APlistClean();
}

/**
 * @brief check and wait for connection
 * 
 */
void smartsensor_wifi::check_connection(void)
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
bool smartsensor_wifi::connected(void)
{
    return WiFi.isConnected();
}

/**
 * @brief Adds ssid and passwords to a list
 * 
 * @param ssid 
 * @param passphrase 
 */
void smartsensor_wifi::addAP(const char* ssid, const char *passphrase)
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
void smartsensor_wifi::APlistClean(void)
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
