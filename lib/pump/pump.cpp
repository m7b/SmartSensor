#include "pump.h"

pump::pump(uint8_t output_pin, uint8_t input_pin, bool on_state, uint32_t delay_ms)
{
    _output_pin = output_pin;
    _input_pin  = input_pin;
    _on_state   = on_state;
    _setup      = false;

    _on_demand  = false;
    _off_demand = false;

    _is_on      = false;
    _is_off     = false;
  
    _step       = 0;
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
    pinMode(_input_pin, INPUT_PULLUP);
    
    //initially switch pump off
    off();

    _setup = true;
}

void pump::loop(void)
{
    bool ret_val;
    if (!conditions_ok())
        return;

    switch (_step)
    {
        case 0: //init

            _on_demand = false;
            _off_demand = false;

            _step += 10;
            break;

        case 10: //wait button OR mqtt
            ret_val = !digitalRead(_input_pin);
            if (ret_val)
            {
                _start_entprell = millis();
                _step += 10;
            }

            if (_on_demand)
            {
                _on_demand = false;
                _step = 30;
            }

            break;

        case 20: //entprell
            ret_val = !digitalRead(_input_pin);
            if (!ret_val)
                _step = 0;

            if (get_duration_ms(_start_entprell) >= 200)
                _step += 10;
            
            break;

        case 30: //on via button OR mqtt
            on();
            _off_demand = false; // if on_demand and off_demand where set
            _start = millis();
            _step += 10;
            break;

        case 40:
            //wait button OR mqtt
            ret_val = !digitalRead(_input_pin);
            if (ret_val)
            {
                _start_entprell = millis();
                _step += 10;
            }

            if (get_duration_ms(_start) >= _delay_ms)
            {
                _step = 60;
            }

            if (_off_demand)
            {
                _off_demand = false;
                _step = 60;
            }

            
            break;

        case 50: //entprell
            ret_val = !digitalRead(_input_pin);
            if (!ret_val)
                _step = 40;

            if (get_duration_ms(_start_entprell) >= 200)
                _step += 10;

            break;

        case 60:
            off();
            _step = 0;
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
}

void pump::off(void)
{
    if (_on_state)
        digitalWrite(_output_pin, LOW);
    else
        digitalWrite(_output_pin, HIGH); 

    _is_on  = false;
    _is_off = true;
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