#ifndef RWS_PUBSUBCLIENT_H
#define RWS_PUBSUBCLIENT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MAX_MSG_SIZE 50

class rws_pubsubclient : public PubSubClient
{
    public:
        rws_pubsubclient();
        ~rws_pubsubclient();

        void check_connection(void);

        bool publish(const char *topic, const unsigned long ul_value);
        bool publish(const char *topic, const float f_value);
        bool publish(const char *topic, const time_t t_value);
        bool publish(const char *topic, const std::string s_value);
        using PubSubClient::publish;

        std::string add_root_topic(const char *topic);

    private:
        WiFiClient espClient;
        String clientId;
        char msg[MAX_MSG_SIZE];
        int max_connection_tries;

        void reconnect(void);
};

#endif // RWS_PUBSUBCLIENT_H