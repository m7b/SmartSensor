#ifndef MQTT_COMM_H
#define MQTT_COMM_H

#include <string>
#include "common/settings/settings.h"

#if defined(DST_BARREL) || defined(SRC_BARREL)
    #include "sensor/settings/settings.h"
#else
    #include "controller/settings/settings.h"
#endif



enum FunctionModes
{
    FUNCTION_MODE_PERMANENT_MEASSURE = 48, //'0'
    FUNCTION_MODE_INTERVAL_MEASURE__5_SEK, //'1'
    FUNCTION_MODE_INTERVAL_MEASURE_10_SEK, //'2'
    FUNCTION_MODE_INTERVAL_MEASURE__5_MIN, //'3'
    FUNCTION_MODE_DEEP_SLEEP_20_SEK,       //'4'
    FUNCTION_MODE_DEEP_SLEEP__5_MIN        //'5'
};

//std::string self_root_topic(const char *topic);



//cmd_topic_to_src
//cmd_topic_to_dst

//ack_topic_from_src
//ack_topic_from_dst

#endif // MQTT_COMM_H