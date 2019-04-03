#ifndef COMMON_TOPICS_H
#define COMMON_TOPICS_H

#include "common/settings/settings.h"

/**
 * @brief Root of topics to publish
 * 
 */
#if defined(SRC_BARREL)
    #define LOCATION_TYPE           LOCATION_TYPE_BARREL_SRC
#elif defined(DST_BARREL)
    #define LOCATION_TYPE           LOCATION_TYPE_BARREL_DST
#else
    #define LOCATION_TYPE           LOCATION_TYPE_CONTROLLER
#endif


/**
 * @brief Topics from sensor
 * 
 */
#define SENS_RAW_CM          "Lvl/raw_cm"
#define SENS_CM              "Lvl/cm"
#define SENS_PERCENT         "Lvl/percent"
#define SENS_TIMESTAMP       "Lvl/timestamp"
#define SENS_LOC_TIMESTAMP   "Lvl/local_timestamp"
#define MODE_ACK             "FunctionModeAck"
#define SENS_LAST_WILL_TOPIC "Status"


/**
 * @brief Topics from controller
 * 
 */
#define MODE_REQ              "FunctionModeReq"
#define MANUAL_PUMP_ACK       "ManualPumpAck"
#define MANUAL_VALVE_ACK      "ManualValveAck"
#define AMBIENT_BRIGHTNESS    "AmbientBrightness"
#define CONTR_LAST_WILL_TOPIC "Status"


/**
 * @brief Topics from dashboard
 * 
 */
#define MANUAL_PUMP_REQ    "ManualPumpReq"
#define MANUAL_VALVE_REQ   "ManualValveReq"


/**
 * @brief Helper to subscribe to topics with apropriate root
 * 
 */
#define TOPIC(NUM, NAME)                 {(const int) NUM, TOP_LEVEL_TOPIC LOCATION_TYPE NAME}
#define TOPIC_FROM_SRC(NUM, NAME)        {(const int) NUM, TOP_LEVEL_TOPIC LOCATION_TYPE_BARREL_SRC NAME}
#define TOPIC_FROM_DST(NUM, NAME)        {(const int) NUM, TOP_LEVEL_TOPIC LOCATION_TYPE_BARREL_DST NAME}
#define TOPIC_FROM_CONTROLLER(NUM, NAME) {(const int) NUM, TOP_LEVEL_TOPIC LOCATION_TYPE_CONTROLLER NAME}
#define TOPIC_FROM_DASHBOARD(NUM, NAME)  {(const int) NUM, TOP_LEVEL_TOPIC DASHBOARD NAME}

#endif // COMMON_TOPICS_H