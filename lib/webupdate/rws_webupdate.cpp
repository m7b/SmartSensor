#include "rws_webupdate.h"


rws_webupdate::rws_webupdate(const char* dns_host, ESP8266WebServer *websrv, ESP8266HTTPUpdateServer *httpupdsrv)
{
    _dns_host = dns_host;
    std::transform(_dns_host.begin(), _dns_host.end(),_dns_host.begin(), ::tolower);

    _web_server = websrv;
    _http_updater = httpupdsrv;
}


rws_webupdate::~rws_webupdate()
{
}


void rws_webupdate::setup(void)
{
    MDNS.begin(_dns_host.c_str());

    _web_server->on("/", [this]() {
        _web_server->send(200, "text/plain", "Hi! I am a Sensor.");
    });

    _http_updater->setup(_web_server);
    _web_server->begin();

    MDNS.addService("http", "tcp", 80);
    Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\r\n", _dns_host.c_str());
}


void rws_webupdate::loop(void)
{
    _web_server->handleClient();
    MDNS.update();
}