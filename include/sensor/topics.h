#ifndef SENSOR_TOPICS_H
#define SENSOR_TOPICS_H

#include "common/topics.h"
#include <vector>

/**
 * @brief Topics to publish
 * 
 */
#define LEVEL_SENSOR_CM_TOPIC       TOP_LEVEL_TOPIC LOCATION_TYPE "Lvl/raw_cm"
#define LEVEL_CM_TOPIC              TOP_LEVEL_TOPIC LOCATION_TYPE "Lvl/cm"
#define LEVEL_PERCENT_TOPIC         TOP_LEVEL_TOPIC LOCATION_TYPE "Lvl/percent"
#define LEVEL_TIMESTAMP_TOPIC       TOP_LEVEL_TOPIC LOCATION_TYPE "Lvl/timestamp"
#define LEVEL_LOCAL_TIMESTAMP_TOPIC TOP_LEVEL_TOPIC LOCATION_TYPE "Lvl/local_timestamp"

#define FUNCTION_MODE_ACK           TOP_LEVEL_TOPIC LOCATION_TYPE "FunctionModeAck"


/**
 * @brief Topics to subscribe
 * 
 */
static const std::vector<std::pair<const int, const char*>> topics_to_subscribe = {
/*  TOPIC(<idx>, "<topic>"), */
    TOPIC(0, "FunctionModeRequest"),
    TOPIC(1, "inTopic")
};

#endif // SENSOR_TOPICS_H