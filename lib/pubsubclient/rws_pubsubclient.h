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

#define ON_CON_FCT_SIGNATURE        std::function<void(void)> on_connect_publish_fct
#define ON_CON_FAILED_FCT_SIGNATURE std::function<void(void)> on_connect_failed_fct

class rws_pubsubclient : public PubSubClient
{
    public:
        rws_pubsubclient(const char *server, uint16_t port, const char *clientId, const char *user, const char *pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage);
        ~rws_pubsubclient();

        void check_connection(void);

        void set_topics_to_subscribe(const std::vector<std::tuple<const int, const char*, const uint8_t>> *topics_to_subscribe);
        void set_on_con_fct(ON_CON_FCT_SIGNATURE);
        void set_on_con_failed_fct(ON_CON_FAILED_FCT_SIGNATURE);

        bool publish(const char *topic, const int i_value);
        bool publish(const char *topic, const uint8_t value);
        bool publish(const char *topic, const unsigned long ul_value);
        bool publish(const char *topic, const float f_value);
        bool publish(const char *topic, const time_t t_value);
        bool publish(const char *topic, const std::string s_value);
        using PubSubClient::publish;

    private:
        WiFiClient espClient;
        const char *_server;
        uint16_t _port;
        const char *_clientId;
        const char *_user;
        const char *_pass;
        const char* _willTopic;
        uint8_t _willQos;
        boolean _willRetain;
        const char* _willMessage;

        bool _cleanSession;

        const std::vector<std::tuple<const int, const char*, const uint8_t>> *_topics_to_subscribe;
        char msg[MAX_MSG_SIZE];

        long _lastReconnectAttempt;

        bool reconnect(void);

        uint8_t _connection_tries;
        ON_CON_FCT_SIGNATURE;
        ON_CON_FAILED_FCT_SIGNATURE;
};

#endif // RWS_PUBSUBCLIENT_H