#ifndef SMARTSENSOR_STATEMACHINE_H
#define SMARTSENSOR_STATEMACHINE_H

#include "smartsensor_barrel.h"

enum stages
{
    PERMANENT_MEASSURE = 0,
    INTERVAL_MEASURE__5_SEK,
    INTERVAL_MEASURE_10_SEK,
    INTERVAL_MEASURE__5_MIN
};

class smartsensor_statemachine
{
    public:
        smartsensor_statemachine(smartsensor_barrel *barrel);
        ~smartsensor_statemachine();

    private:
        smartsensor_barrel *barrel;
};

#endif // SMARTSENSOR_STATEMACHINE_H