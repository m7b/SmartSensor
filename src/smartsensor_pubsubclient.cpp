#include "smartsensor_pubsubclient.h"
#include "settings/smartsensor_settings.h"

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
bool smartsensor_pubsubclient::publish(const char *topic, unsigned long ul_value)
{
    std::string t = add_root_topic(topic);

    char buffer [20];
    int ret = snprintf(buffer, sizeof(buffer), "%ld", ul_value);
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(t.c_str(), buffer, MQTT_MESSAGES_RETAINED);
    }

    return ret;
}


bool smartsensor_pubsubclient::publish(const char *topic, float f_value)
{
    std::string t = add_root_topic(topic);

    char buffer [20];
    int ret = snprintf(buffer, sizeof(buffer), "%.2f", f_value);
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(t.c_str(), buffer, MQTT_MESSAGES_RETAINED);
    }

    return ret;
}


bool smartsensor_pubsubclient::publish(const char *topic, time_t t_value)
{
    std::string t = add_root_topic(topic);

    char buffer [20];
    int ret = snprintf(buffer, sizeof(buffer), "%ld", t_value);
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(t.c_str(), buffer, MQTT_MESSAGES_RETAINED);
    }

    return ret;
}

/**
 * @brief Reconnect to MQTT publisher
 * 
 */
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
        Serial.println("MQTT connected");
        Serial.println("CLient ID is: " + clientId);
        // Once connected, publish an announcement...
        publish("outTopic", "hello world");
        // ... and resubscribe
        subscribe("inTopic");
    }
    else
    {
        Serial.printf("failed, rc=%d, try again in 5 seconds.\r\n", state());
        // Wait 5 seconds before retrying
        delay(5000);
    }
}


std::string smartsensor_pubsubclient::add_root_topic(const char *topic)
{
    return std::string(TOP_LEVEL_TOPIC) + std::string(LOCATION_NAME_SENSOR) + topic;
}