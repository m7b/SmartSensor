#include "pump.h"

pump::pump(uint8_t output_pin, bool on_state, uint32_t delay_ms)
: statemachine(N000_PUMP_INIT_STEP)
{
    setCallback_on(NULL);
    setCallback_off(NULL);

    _output_pin = output_pin;
    _on_state   = on_state;
    _setup      = false;

    _on_demand  = false;
    _off_demand = false;

    _is_on      = false;
    _is_off     = false;
  
    _delay_ms   = delay_ms; //30000; //300000; //300000ms => 5min
}

pump::~pump()
{
}

void pump::setup(void)
{
    //if (_output_pin == 0)
    //    return;

    pinMode(_output_pin, OUTPUT);
    
    //initially switch pump off
    off();

    _setup = true;
}

void pump::loop(void)
{
    if (!conditions_ok())
        return;

    switch (get_step())
    {
        case N000_PUMP_INIT_STEP:
            _on_demand = false;
            _off_demand = false;

            set_next_step(N010_PUMP_WAIT_FOR_ON_DEMAND_STEP);
            break;

        case N010_PUMP_WAIT_FOR_ON_DEMAND_STEP:
            if (_on_demand)
            {
                _on_demand = false;
                set_next_step(N020_PUMP_SWITCH_ON_STEP);
            }
            break;

        case N020_PUMP_SWITCH_ON_STEP:
            on();
            _off_demand = false; // if on_demand and off_demand where set
            _start = millis();
            set_next_step(N030_PUMP_WAIT_FOR_OFF_DEMAND_STEP);
            break;

        case N030_PUMP_WAIT_FOR_OFF_DEMAND_STEP:
            //wait delay
            if (get_duration_ms(_start) >= _delay_ms)
            {
                set_next_step(N040_PUMP_SWITCH_OFF_STEP);
            }
            //wait _off_demand
            if (_off_demand)
            {
                _off_demand = false;
                set_next_step(N040_PUMP_SWITCH_OFF_STEP);
            }
            break;

        case N040_PUMP_SWITCH_OFF_STEP:
            off();
            set_next_step(N999_PUMP_END);
            break;

        case N999_PUMP_END:
            set_next_step(N000_PUMP_INIT_STEP);
            break;
    }
}


bool pump::conditions_ok(void)
{
    if (!_setup)
        return false;

    return true;
}

void pump::on(void)
{
    if (_on_state)
        digitalWrite(_output_pin, HIGH);
    else
        digitalWrite(_output_pin, LOW);

    _is_on  = true;
    _is_off = false;

    if (callback_on)
        callback_on();
}

void pump::off(void)
{
    if (_on_state)
        digitalWrite(_output_pin, LOW);
    else
        digitalWrite(_output_pin, HIGH); 

    _is_on  = false;
    _is_off = true;

    if (callback_off)
        callback_off();
}


void pump::set_on_demand(void)
{
    _on_demand = true;
}


void pump::set_off_demand(void)
{
    _off_demand = true;
}


bool pump::is_on(void)
{
    return _is_on;
}


bool pump::is_off(void)
{
    return _is_off;
}

void pump::setCallback_on(std::function<void(void)> callback_on)
{
    this->callback_on = callback_on;
}

void pump::setCallback_off(std::function<void(void)> callback_off)
{
    this->callback_off = callback_off;
}