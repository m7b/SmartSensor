#ifndef MQTT_COMM_H
#define MQTT_COMM_H

#include <string>
#include "common/settings/settings.h"

#if defined(DST_BARREL) || defined(SRC_BARREL)
    #include "sensor/settings/settings.h"
#else
    #include "controller/settings/settings.h"
#endif

std::string add_root_topic(const char *topic);

//cmd_topic_to_src
//cmd_topic_to_dst

//ack_topic_from_src
//ack_topic_from_dst

#endif // MQTT_COMM_H