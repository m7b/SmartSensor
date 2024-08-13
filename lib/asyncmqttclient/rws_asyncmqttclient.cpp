#include "rws_asyncmqttclient.h"

rws_asyncmqttclient::rws_asyncmqttclient(const char *server, uint16_t port, const char *clientId, const char *user, const char *pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage)
{
    _server = server;
    _port   = port;
    _clientId = clientId;
    _user = user;
    _pass = pass;

    set_onMqttConnect(NULL);
    set_onMqttDisconnect(NULL);
    set_onMqttSubscribe(NULL);
    set_onMqttUnsubscribe(NULL);
    set_onMqttMessage(NULL);
    set_onMqttPublish(NULL);


    _willTopic = willTopic;
    _willQos = willQos;
    _willRetain = willRetain;
    _willMessage = willMessage;

    _cleanSession = true;
}

rws_asyncmqttclient::~rws_asyncmqttclient()
{
}

void rws_asyncmqttclient::set_topics_to_subscribe(const std::vector<std::tuple<const int, const char*, const uint8_t>> *topics_to_subscribe)
{
    _topics_to_subscribe = topics_to_subscribe;
}

void rws_asyncmqttclient::do_subscribe(void)
{
    for(auto topic : *_topics_to_subscribe)
        subscribe(std::get<TP_TOP>(topic), std::get<TP_QOS>(topic));
}

void rws_asyncmqttclient::set_onMqttConnect(std::function<void(bool sessionPresent)> onMqttConnect)
{
    this->onMqttConnect = onMqttConnect;
    onConnect(onMqttConnect);
}

void rws_asyncmqttclient::set_onMqttDisconnect(std::function<void(AsyncMqttClientDisconnectReason reason)> onMqttDisconnect)
{
    this->onMqttDisconnect = onMqttDisconnect;
    onDisconnect(onMqttDisconnect);
}

void rws_asyncmqttclient::set_onMqttSubscribe(std::function<void(uint16_t packetId, uint8_t qos)> onMqttSubscribe)
{
    this->onMqttSubscribe = onMqttSubscribe;
    onSubscribe(onMqttSubscribe);
}

void rws_asyncmqttclient::set_onMqttUnsubscribe(std::function<void(uint16_t packetId)> onMqttUnsubscribe)
{
    this->onMqttUnsubscribe = onMqttUnsubscribe;
    onUnsubscribe(onMqttUnsubscribe);
}

void rws_asyncmqttclient::set_onMqttMessage(std::function<void(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)> onMqttMessage)
{
    this->onMqttMessage = onMqttMessage;
    onMessage(onMqttMessage);
}

void rws_asyncmqttclient::set_onMqttPublish(std::function<void(uint16_t packetId)> onMqttPublish)
{
    this->onMqttPublish = onMqttPublish;
    onPublish(onMqttPublish);
}

void rws_asyncmqttclient::set_server(void)
{
    setServer(_server, _port);
}