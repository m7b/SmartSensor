#ifndef CONTROLLER_TOPICS_H
#define CONTROLLER_TOPICS_H

#include "common/topics.h"
#include <vector>

/**
 * @brief Topics to publish
 * 
 */
#define FUNCTION_MODE_SRC_REQUEST TOP_LEVEL_TOPIC LOCATION_TYPE_BARREL_SRC MODE_REQ
#define FUNCTION_MODE_DST_REQUEST TOP_LEVEL_TOPIC LOCATION_TYPE_BARREL_DST MODE_REQ
#define MANUAL_PUMP_ACKNOWLEDGE   TOP_LEVEL_TOPIC DASHBOARD MANUAL_PUMP_ACK
#define MANUAL_VALVE_ACKNOWLEDGE  TOP_LEVEL_TOPIC DASHBOARD MANUAL_VALVE_ACK
#define VAL_AMBIENT_BRIGHTNESS    TOP_LEVEL_TOPIC LOCATION_TYPE_CONTROLLER AMBIENT_BRIGHTNESS
#define VAL_LIFE_SIGN             TOP_LEVEL_TOPIC LOCATION_TYPE_CONTROLLER LIFE_SIGN

#define LAST_WILL_TOPIC           TOP_LEVEL_TOPIC LOCATION_TYPE_CONTROLLER CONTR_LAST_WILL_TOPIC


/**
 * @brief Topics to subscribe
 * 
 */
static const std::vector<std::tuple<const int, const char*, const uint8_t>> topics_to_subscribe = {
// TOPIC_FROM_SRC(NUM, NAME, QOS)
// TOPIC_FROM_DST(NUM, NAME, QOS)
// TOPIC_FROM_CONTROLLER(NUM, NAME, QOS) 
// TOPIC_FROM_DASHBOARD(NUM, NAME, QOS)

/*  TOPIC(<idx>, "<topic>", <qos>), */
    TOPIC_FROM_DASHBOARD(500, MANUAL_PUMP_REQ, QOS1),
    TOPIC_FROM_DASHBOARD(501, MANUAL_VALVE_REQ, QOS1)
};

#endif // CONTROLLER_TOPICS_H