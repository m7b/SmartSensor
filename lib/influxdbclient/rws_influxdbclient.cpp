#include "rws_influxdbclient.h"

//rws_mqttclient::rws_mqttclient(const char *server, uint16_t port, const char *clientId, const char *user, const char *pass, const char* willTopic, uint8_t willQos, bool willRetain, const char* willMessage)
rws_influxdbclient::rws_influxdbclient(const String &influxdb_url, const String &influxdb_org, const String &influxdb_bucket, const String &influxdb_token)
:InfluxDBClient(influxdb_url, influxdb_org, influxdb_bucket, influxdb_token, InfluxDbCloud2CACert)
{
    _influxdb_url = &influxdb_url;
    _influxdb_org = &influxdb_org;
    _influxdb_bucket = &influxdb_bucket;
    _influxdb_token = &influxdb_token;
}

rws_influxdbclient::~rws_influxdbclient()
{
}