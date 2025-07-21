#ifndef RWS_MQTTCLIENT_H
#define RWS_MQTTCLIENT_H

#include "rws_common.h"

#include <PicoMQTT.h>

#define MAX_MSG_SIZE 50

/**
 * @brief Helper to get the values from tuple
 *
 */
#define TP_NUM 0
#define TP_TOP 1
#define TP_QOS 2

class rws_mqttclient : public PicoMQTT::Client
{
    public:
        rws_mqttclient(const char *server, uint16_t port, const char *clientId, const char *user, const char *pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage);
        ~rws_mqttclient();
        
        void set_topics_to_subscribe(const std::vector<std::tuple<const int, const char*, const uint8_t>> *topics_to_subscribe);
        void do_subscribe(void);

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

};

#endif // RWS_MQTTCLIENT_H