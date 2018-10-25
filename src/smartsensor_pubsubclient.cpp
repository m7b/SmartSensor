#include "smartsensor_pubsubclient.h"

#define MQTT_SERVER "192.168.178.36"  //bierfass
#define MQTT_PORT   1883

smartsensor_pubsubclient::smartsensor_pubsubclient()
: PubSubClient(espClient)
{
    setServer(MQTT_SERVER, MQTT_PORT);
//  setCallback(callback);

    clientId = "";
    max_connection_tries = 3;
}

smartsensor_pubsubclient::~smartsensor_pubsubclient()
{
}

void smartsensor_pubsubclient::check_connection(void)
{
    int try_counter = 0;
    while (!connected() && (try_counter < max_connection_tries))
    {
        reconnect();
        try_counter++;
    }
}

/**
 * @brief Publish an unsigned long value to specific topic
 * 
 * @param topic 
 * @param value 
 * @return true 
 * @return false 
 */
bool smartsensor_pubsubclient::publish(const char *topic, unsigned long value)
{
    char buffer [20];
    int ret = snprintf(buffer, sizeof(buffer), "%ld\n", value);
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(topic, buffer);
    }

    return ret;
}

void smartsensor_pubsubclient::reconnect(void)
{
    Serial.print("Attempting MQTT connection to ");
    Serial.print(MQTT_SERVER);
    Serial.print(":");
    Serial.print(MQTT_PORT);
    Serial.println(" ... ");

    // Create a random client ID
    clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    // Attempt to connect
    if (connect(clientId.c_str()))

    {
        Serial.println("connected");
        // Once connected, publish an announcement...
        publish("outTopic", "hello world");
        // ... and resubscribe
        subscribe("inTopic");
    }
    else
    {
        Serial.printf("failed, rc=%d, try again in 5 seconds.\n", state());
        // Wait 5 seconds before retrying
        delay(5000);
    }
}