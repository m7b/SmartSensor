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


/**
 * @brief Topics to subscribe
 * 
 */
static const std::vector<std::pair<const int, const char*>> topics_to_subscribe = {
/*  TOPIC(<idx>, "<topic>"), */
    TOPIC(0, MODE_REQ),
    TOPIC(1, "inTopic")
};

#endif // SENSOR_TOPICS_H