#ifndef SMARTSENSOR_STATEMACHINE_H
#define SMARTSENSOR_STATEMACHINE_H

#include "smartsensor_barrel.h"

enum stages
{
    PERMANENT_MEASSURE = 48, //'0'
    INTERVAL_MEASURE__5_SEK, //'1'
    INTERVAL_MEASURE_10_SEK, //'2'
    INTERVAL_MEASURE__5_MIN  //'3'
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