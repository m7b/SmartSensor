#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <rws_common.h>
#include "sensor/barrel.h"

enum stages
{
    PERMANENT_MEASSURE = 48, //'0'
    INTERVAL_MEASURE__5_SEK, //'1'
    INTERVAL_MEASURE_10_SEK, //'2'
    INTERVAL_MEASURE__5_MIN, //'3'
    DEEP_SLEEP_20_SEK,       //'4'
    DEEP_SLEEP__5_MIN        //'5'
};

class statemachine
{
    public:
        statemachine(barrel *bar);
        ~statemachine();

        void loop(int operation_mode);

    private:
        barrel *bar;

        int step;

        unsigned long start_time;
        unsigned long ds_time;
        
        void set_next_step(int step);
        int get_step(void);
};

#endif // STATEMACHINE_H