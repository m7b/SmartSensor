#include "rws_asyncmqttclient.h"

rws_asyncmqttclient::rws_asyncmqttclient(const char *server, uint16_t port, const char *clientId, const char *user, const char *pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage)
{
    _server = server;
    _port   = port;
    _clientId = clientId;
    _user = user;
    _pass = pass;
    setServer(server, port);

    _willTopic = willTopic;
    _willQos = willQos;
    _willRetain = willRetain;
    _willMessage = willMessage;

    _cleanSession = true;
}

rws_asyncmqttclient::~rws_asyncmqttclient()
{
}