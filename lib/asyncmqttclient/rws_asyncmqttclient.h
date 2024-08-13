#ifndef RWS_ASYNCMQTTCLIENT_H
#define RWS_ASYNCMQTTCLIENT_H

#include "rws_common.h"

#include <AsyncMqttClient.h>

#define MAX_MSG_SIZE 50

/**
 * @brief Helper to get the values from tuple
 *
 */
#define TP_NUM 0
#define TP_TOP 1
#define TP_QOS 2

class rws_asyncmqttclient : public AsyncMqttClient
{
    public:
        rws_asyncmqttclient(const char *server, uint16_t port, const char *clientId, const char *user, const char *pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage);
        ~rws_asyncmqttclient();
        
        Ticker mqttReconnectTimer;

        void set_topics_to_subscribe(const std::vector<std::tuple<const int, const char*, const uint8_t>> *topics_to_subscribe);
        void do_subscribe(void);

        void set_onMqttConnect(std::function<void(bool sessionPresent)> onMqttConnect);
        void set_onMqttDisconnect(std::function<void(AsyncMqttClientDisconnectReason reason)> onMqttDisconnect);
        void set_onMqttSubscribe(std::function<void(uint16_t packetId, uint8_t qos)> onMqttSubscribe);
        void set_onMqttUnsubscribe(std::function<void(uint16_t packetId)> onMqttUnsubscribe);
        void set_onMqttMessage(std::function<void(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)> onMqttMessage);
        void set_onMqttPublish(std::function<void(uint16_t packetId)> onMqttPublish);
        void set_server(void);

    private:
        const char *_server;
        uint16_t _port;
        const char *_clientId;
        const char *_user;
        const char *_pass;
        const char* _willTopic;
        uint8_t _willQos;
        boolean _willRetain;
        const char* _willMessage;

        const std::vector<std::tuple<const int, const char*, const uint8_t>> *_topics_to_subscribe;
        bool _cleanSession;

        std::function<void(bool sessionPresent)> onMqttConnect;
        std::function<void(AsyncMqttClientDisconnectReason reason)> onMqttDisconnect;
        std::function<void(uint16_t packetId, uint8_t qos)> onMqttSubscribe;
        std::function<void(uint16_t packetId)> onMqttUnsubscribe;
        std::function<void(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)> onMqttMessage;
        std::function<void(uint16_t packetId)> onMqttPublish;
};

#endif // RWS_ASYNCMQTTCLIENT_H