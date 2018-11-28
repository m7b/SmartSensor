#ifndef CONTROLLER_TOPICS_H
#define CONTROLLER_TOPICS_H

#include "common/topics.h"
#include <vector>

/**
 * @brief Topics to publish
 * 
 */
#define LEVEL_SENSOR_CM_TOPIC       "Lvl/raw_cm"
#define LEVEL_CM_TOPIC              "Lvl/cm"
#define LEVEL_PERCENT_TOPIC         "Lvl/percent"
#define LEVEL_TIMESTAMP_TOPIC       "Lvl/timestamp"
#define LEVEL_LOCAL_TIMESTAMP_TOPIC "Lvl/local_timestamp"


/**
 * @brief Topics to subscribe
 * 
 */
static const std::vector<std::pair<const int, const char*>> topics_to_subscribe = {
/*  TOPIC(<idx>, "<topic>"), */
    TOPIC(0, "FunctionMode"),
    TOPIC(1, "inTopic")
};

#endif // CONTROLLER_TOPICS_H

