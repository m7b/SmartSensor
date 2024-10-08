#ifndef RWS_WEBUPDATE_H
#define RWS_WEBUPDATE_H

#include "rws_common.h"

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

class rws_webupdate
{
    public:
        rws_webupdate(const char* dns_host, ESP8266WebServer *websrv, ESP8266HTTPUpdateServer *httpupdsrv, const char *clientId);
        ~rws_webupdate();

        void setup(void);
        void loop(void);

    private:
        std::string _dns_host;
        ESP8266WebServer *_web_server;
        ESP8266HTTPUpdateServer *_http_updater;
        const char *_clientId;
};

#endif // RWS_WEBUPDATE_H