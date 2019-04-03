#ifndef SENSOR_TOPICS_H
#define SENSOR_TOPICS_H

#include "common/topics.h"
#include <vector>

/**
 * @brief Topics to publish
 * 
 */
#define LEVEL_SENSOR_CM_TOPIC       TOP_LEVEL_TOPIC LOCATION_TYPE SENS_RAW_CM
#define LEVEL_CM_TOPIC              TOP_LEVEL_TOPIC LOCATION_TYPE SENS_CM
#define LEVEL_PERCENT_TOPIC         TOP_LEVEL_TOPIC LOCATION_TYPE SENS_PERCENT
#define LEVEL_TIMESTAMP_TOPIC       TOP_LEVEL_TOPIC LOCATION_TYPE SENS_TIMESTAMP
#define LEVEL_LOCAL_TIMESTAMP_TOPIC TOP_LEVEL_TOPIC LOCATION_TYPE SENS_LOC_TIMESTAMP

#define FUNCTION_MODE_ACK           TOP_LEVEL_TOPIC LOCATION_TYPE MODE_ACK
#define LAST_WILL_TOPIC             TOP_LEVEL_TOPIC LOCATION_TYPE SENS_LAST_WILL_TOPIC


/**
 * @brief Topics to subscribe
 * 
 */
static const std::vector<std::tuple<const int, const char*, const uint8_t>> topics_to_subscribe = {
/*  TOPIC(<idx>, "<topic>", <qos>), */
    TOPIC(0, MODE_REQ, QOS1),
    TOPIC(1, "inTopic", QOS0)
};

#endif // SENSOR_TOPICS_H