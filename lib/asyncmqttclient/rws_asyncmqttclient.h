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

        bool _cleanSession;
};

#endif // RWS_ASYNCMQTTCLIENT_H