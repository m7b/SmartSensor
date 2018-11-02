#ifndef SMARTSENSOR_STATEMACHINE_H
#define SMARTSENSOR_STATEMACHINE_H

#include "smartsensor_barrel.h"

enum stages
{
    PERMANENT_MEASSURE = 48, //'0'
    INTERVAL_MEASURE__5_SEK, //'1'
    INTERVAL_MEASURE_10_SEK, //'2'
    INTERVAL_MEASURE__5_MIN, //'3'
    DEEP_SLEEP_20_SEK,       //'4'
    DEEP_SLEEP__5_MIN        //'5'
};

class smartsensor_statemachine
{
    public:
        smartsensor_statemachine(smartsensor_barrel *barrel);
        ~smartsensor_statemachine();

        void loop(int operation_mode);

    private:
        smartsensor_barrel *barrel;

        int step;

        unsigned long start_time;
        
        void set_next_step(int step);
        int get_step(void);

        unsigned long get_duration(unsigned long start);
        unsigned long get_duration_us(unsigned long start);

        unsigned long ds_time;
};

#endif // SMARTSENSOR_STATEMACHINE_H