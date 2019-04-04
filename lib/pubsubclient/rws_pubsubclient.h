#ifndef RWS_PUBSUBCLIENT_H
#define RWS_PUBSUBCLIENT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MAX_MSG_SIZE 50

/**
 * @brief Helper to get the values from tuple
 *
 */
#define TP_NUM 0
#define TP_TOP 1
#define TP_QOS 2

class rws_pubsubclient : public PubSubClient
{
    public:
        rws_pubsubclient(const char *server, uint16_t port, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage);
        ~rws_pubsubclient();

        void check_connection(void);

        void set_topics_to_subscribe(const std::vector<std::tuple<const int, const char*, const uint8_t>> *topics_to_subscribe);

        bool publish(const char *topic, const int i_value);
        bool publish(const char *topic, const uint8_t value);
        bool publish(const char *topic, const unsigned long ul_value);
        bool publish(const char *topic, const float f_value);
        bool publish(const char *topic, const time_t t_value);
        bool publish(const char *topic, const std::string s_value);
        using PubSubClient::publish;

    private:
        WiFiClient espClient;
        const char *server;
        uint16_t port;
        const char* _willTopic;
        uint8_t _willQos;
        boolean _willRetain;
        const char* _willMessage;

        String clientId;
        const std::vector<std::tuple<const int, const char*, const uint8_t>> *_topics_to_subscribe;
        char msg[MAX_MSG_SIZE];
        int max_connection_tries;

        void reconnect(void);
};

#endif // RWS_PUBSUBCLIENT_H