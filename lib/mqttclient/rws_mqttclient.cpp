#include "rws_mqttclient.h"

rws_mqttclient::rws_mqttclient(const char *server, uint16_t port, const char *clientId, const char *user, const char *pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage)
:Client(server, port, clientId, user, pass)
{
    _server = server;
    _port   = port;
    _clientId = clientId;
    _user = user;
    _pass = pass;

    _willTopic = willTopic;
    _willQos = willQos;
    _willRetain = willRetain;
    _willMessage = willMessage;

    _cleanSession = true;
}

rws_mqttclient::~rws_mqttclient()
{
}

void rws_mqttclient::set_topics_to_subscribe(const std::vector<std::tuple<const int, const char*, const uint8_t>> *topics_to_subscribe)
{
    _topics_to_subscribe = topics_to_subscribe;
}

void rws_mqttclient::do_subscribe(void)
{
    for(auto topic : *_topics_to_subscribe)
        subscribe(std::get<TP_TOP>(topic));
        //subscribe(std::get<TP_TOP>(topic), std::get<TP_QOS>(topic));
}