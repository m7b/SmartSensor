#ifndef MQTT_COMM_H
#define MQTT_COMM_H

#include <string>
#include "common/settings/settings.h"

#if defined(DST_BARREL) || defined(SRC_BARREL)
    #include "sensor/settings/settings.h"
#else
    #include "controller/settings/settings.h"
#endif


const static unsigned long pm_time_200ms =    200; //base is ms
const static unsigned long im_time__5sec =   5000;
const static unsigned long im_time_10sec =  10000;
const static unsigned long im_time__1min =  60000;
const static unsigned long im_time__5min = 300000;
const static uint64_t ds_time_20sec =   20e6; //base is us
const static uint64_t ds_time__1min =    6e7;
const static uint64_t ds_time__5min =    3e8;
const static uint64_t ds_time_10min =    6e8;
const static uint64_t ds_time_30min =   18e8;
const static uint64_t ds_time__1std =   36e9;

//std::string self_root_topic(const char *topic);



//cmd_topic_to_src
//cmd_topic_to_dst

//ack_topic_from_src
//ack_topic_from_dst

#endif // MQTT_COMM_H