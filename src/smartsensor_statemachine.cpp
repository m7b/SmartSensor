#include "smartsensor_statemachine.h"

smartsensor_statemachine::smartsensor_statemachine(smartsensor_barrel *barrel)
: barrel(barrel)
{
    step = 0;
}
 
 
smartsensor_statemachine::~smartsensor_statemachine()
{
}


void smartsensor_statemachine::set_next_step(int step)
{
    this->step = step;
    start_time = millis();
}


int smartsensor_statemachine::get_step(void)
{
    return step;
}

unsigned long smartsensor_statemachine::get_duration(unsigned long start)
{
    unsigned long act = millis();
    unsigned long max = 0xffffffff;

    if (start > act)
        return  max - start + act;

    return act - start;
}

unsigned long smartsensor_statemachine::get_duration_us(unsigned long start)
{
    unsigned long act = micros();
    unsigned long max = 0xffffffff;

    if (start > act)
        return  max - start + act;

    return act - start;
}

void smartsensor_statemachine::loop(int operation_mode)
{
    switch (get_step())
    {
        case 0:
            //init
            set_next_step(10);
            break;

        case 10:
            //start timeout
            start_time = millis();
            set_next_step(20);
            break;

        case 20:
            //start meassure
            barrel->do_measure();
            set_next_step(30);
            break;

        case 30:
            //report meassure
            barrel->do_publish();
            set_next_step(40);
            break;

        case 40:
            //wait timeout depending on operation mode
            unsigned long duration = 0;
            switch (operation_mode)
            {
                case PERMANENT_MEASSURE:      duration =    200; break;
                case INTERVAL_MEASURE__5_SEK: duration =   5000; break;
                case INTERVAL_MEASURE_10_SEK: duration =  10000; break;
                case INTERVAL_MEASURE__5_MIN: duration = 300000; break;
                default:                      duration =   5000; break;
            }

            if (get_duration(start_time) >= duration)
            {
                set_next_step(0);
                break;
            }

    }
}
