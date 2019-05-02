#include "rws_pubsubclient.h"

rws_pubsubclient::rws_pubsubclient(const char *server, uint16_t port, const char *clientId, const char *user, const char *pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage)
: PubSubClient(espClient)
{
    _server = server;
    _port   = port;
    _clientId    = clientId;
    _user = user;
    _pass = pass;
    setServer(server, port);
//  setCallback(callback);

    _willTopic = willTopic;
    _willQos = willQos;
    _willRetain = willRetain;
    _willMessage = willMessage;

    _cleanSession = true;
    
    _lastReconnectAttempt = 0;
    _connection_tries = 0;
}

rws_pubsubclient::~rws_pubsubclient()
{
}

void rws_pubsubclient::set_topics_to_subscribe(const std::vector<std::tuple<const int, const char*, const uint8_t>> *topics_to_subscribe)
{
    _topics_to_subscribe = topics_to_subscribe;
}

void rws_pubsubclient::set_on_con_fct(ON_CON_FCT_SIGNATURE)
{
    this->on_connect_publish_fct = on_connect_publish_fct;
}

void rws_pubsubclient::set_on_con_failed_fct(ON_CON_FAILED_FCT_SIGNATURE)
{
    this->on_connect_failed_fct = on_connect_failed_fct;
}

void rws_pubsubclient::check_connection(void)
{
    if (!connected())
    {
        long now = millis();
        if (now - _lastReconnectAttempt > 5000)
        {
            _lastReconnectAttempt = now;
            // Attempt to reconnect
            if (reconnect()) {
                _lastReconnectAttempt = 0;
                _connection_tries = 0;
            }
            else
            {
                _connection_tries++;
                // callback for connection failed
                if (on_connect_failed_fct && (_connection_tries >= 5))
                    on_connect_failed_fct();
            }
        }
    }
    else
    {
        // Client connected
        
        //keep mqtt alive
        loop();
    }
}



bool rws_pubsubclient::publish(const char *topic, const int i_value)
{
    char buffer [20];
    int ret = snprintf(buffer, sizeof(buffer), "%d", i_value);
    if (ret >= 0 && ret < (int)sizeof(buffer))
    {
        ret = publish(topic, buffer, true);
    }

    return ret;
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
bool rws_pubsubclient::reconnect(void)
{
    Serial.print("Attempting MQTT connection to ");
    Serial.print(_server);
    Serial.print(":");
    Serial.print(_port);
    Serial.println(" ... ");

    // Attempt to connect
    if (connect(_clientId, _user, _pass, _willTopic, _willQos, _willRetain, _willMessage, _cleanSession))
    {
        Serial.println("MQTT connected");
        Serial.printf("Client ID is: %s\r\n", _clientId);

        // Once connected, publish an announcement...
        if (_willTopic)
            publish(_willTopic, "Online", _willRetain);

        // Publish other things
        if (on_connect_publish_fct)
            on_connect_publish_fct();

        // ... and resubscribe
        for(auto topic : *_topics_to_subscribe)
        subscribe(std::get<TP_TOP>(topic), std::get<TP_QOS>(topic));
    }
    else
    {
        Serial.printf("MQTT connection failed, rc=%d.\r\n", state());
        Serial.printf("  - Client Id: %s\r\n", _clientId);
        Serial.printf("  - User: %s\r\n", _user);
    }

    return connected();
}