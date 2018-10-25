#ifndef SMARTSENSOR_PUBSUBCLIENT_H
#define SMARTSENSOR_PUBSUBCLIENT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MAX_MSG_SIZE 50

class smartsensor_pubsubclient : public PubSubClient
{
    public:
        smartsensor_pubsubclient();
        ~smartsensor_pubsubclient();

        void check_connection(void);

        bool publish(const char *topic, unsigned long value);
        using PubSubClient::publish;

    private:
        WiFiClient espClient;
        String clientId;
        char msg[MAX_MSG_SIZE];
        int max_connection_tries;

        void reconnect(void);
};

#endif // SMARTSENSOR_PUBSUBCLIENT_H