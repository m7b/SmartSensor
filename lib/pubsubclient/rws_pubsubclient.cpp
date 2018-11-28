#include "rws_pubsubclient.h"

rws_pubsubclient::rws_pubsubclient(const char *server, uint16_t port)
: PubSubClient(espClient)
{
    this->server = server;
    this->port   = port;
    setServer(server, port);
//  setCallback(callback);

    clientId = "";

    max_connection_tries = 3;
}

rws_pubsubclient::~rws_pubsubclient()
{
}

void rws_pubsubclient::set_topics_to_subscribe(const std::vector<std::pair<const int, const char*>> *topics_to_subscribe)
{
    _topics_to_subscribe = topics_to_subscribe;
}

void rws_pubsubclient::check_connection(void)
{
    int try_counter = 0;
    while (!connected() && (try_counter < max_connection_tries))
    {
        reconnect();
        try_counter++;
    }
}


bool rws_pubsubclient::publish(const char *topic, const uint8_t value)
{
    char buffer [20];
    int ret = snprintf(buffer, sizeof(buffer), "%c", value);
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(topic, buffer, true);
    }

    return ret;
}


/**
 * @brief Publish an unsigned long value to specific topic
 * 
 * @param topic 
 * @param value 
 * @return true 
 * @return false 
 */
bool rws_pubsubclient::publish(const char *topic, const unsigned long ul_value)
{
    char buffer [20];
    int ret = snprintf(buffer, sizeof(buffer), "%ld", ul_value);
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(topic, buffer, true);
    }

    return ret;
}


bool rws_pubsubclient::publish(const char *topic, const float f_value)
{
    char buffer [20];
    int ret = snprintf(buffer, sizeof(buffer), "%.2f", f_value);
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(topic, buffer, true);
    }

    return ret;
}


bool rws_pubsubclient::publish(const char *topic, const time_t t_value)
{
    char buffer [20];
    int ret = snprintf(buffer, sizeof(buffer), "%ld", t_value);
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(topic, buffer, true);
    }

    return ret;
}


bool rws_pubsubclient::publish(const char *topic, const std::string s_value)
{
    char buffer [80];
    int ret = snprintf(buffer, sizeof(buffer), "%s", s_value.c_str());
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(topic, buffer, true);
    }

    return ret;
}

/**
 * @brief Reconnect to MQTT publisher
 * 
 */
void rws_pubsubclient::reconnect(void)
{
    Serial.print("Attempting MQTT connection to ");
    Serial.print(server);
    Serial.print(":");
    Serial.print(port);
    Serial.println(" ... ");

    // Create a random client ID
    clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    // Attempt to connect
    if (connect(clientId.c_str()))

    {
        Serial.println("MQTT connected");
        Serial.println("CLient ID is: " + clientId);
        // Once connected, publish an announcement...
        //publish("outTopic", "hello world");
        // ... and resubscribe
        for(auto topic : *_topics_to_subscribe) 
            subscribe(topic.second);
    }
    else
    {
        Serial.printf("failed, rc=%d, try again in 5 seconds.\r\n", state());
        // Wait 5 seconds before retrying
        delay(5000);
    }
}