#ifndef RWS_INFLUXDBCLIENT_H
#define RWS_INFLUXDBCLIENT_H

#include "rws_common.h"

#include <InfluxDbClient.h>

class rws_influxdbclient : public InfluxDBClient
{
    public:
        rws_influxdbclient(const String &serverUrl, const String &org, const String &bucket, const String &authToken);
        ~rws_influxdbclient();

    private:
        const String *_influxdb_url;
        const String *_influxdb_org;
        const String *_influxdb_bucket;
        const String *_influxdb_token;

};

#endif // RWS_INFLUXDBCLIENT_H