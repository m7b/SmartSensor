#ifndef COMMON_TOPICS_H
#define COMMON_TOPICS_H

#include "common/settings/settings.h"

/**
 * @brief Root of topics to publish
 * 
 */
#if defined(DST_BARREL)
    #define LOCATION_TYPE           LOCATION_TYPE_BARREL_DST
#elif defined(SRC_BARREL)
    #define LOCATION_TYPE           LOCATION_TYPE_BARREL_SRC
#else
    #define LOCATION_TYPE           LOCATION_TYPE_CONTROLLER
#endif


/**
 * @brief Helper to subscribe to topics with apropriate root
 * 
 */
#define TOPIC(NUM, NAME) {(const int) NUM, TOP_LEVEL_TOPIC LOCATION_TYPE NAME}

#endif // COMMON_TOPICS_H