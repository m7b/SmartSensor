#ifndef SMARTSENSOR_STATEMACHINE_H
#define SMARTSENSOR_STATEMACHINE_H

#include "smartsensor_barrel.h"

class smartsensor_statemachine
{
    public:
        smartsensor_statemachine(smartsensor_barrel *barrel);
        ~smartsensor_statemachine();

    private:
        smartsensor_barrel *barrel;
};

#endif // SMARTSENSOR_STATEMACHINE_H