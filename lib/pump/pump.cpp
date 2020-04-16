#include "pump.h"

pump::pump(uint8_t output_pin, uint8_t input_pin, bool on_state)
{
    _output_pin = output_pin;
    _input_pin  = input_pin;
    _on_state   = on_state;
    _setup      = false;
    _step       = 0;
    _delay_ms   = 5000;
}

pump::~pump()
{
}

void pump::setup(void)
{
    //if (_output_pin == 0)
    //    return;

    pinMode(_output_pin, OUTPUT);
    pinMode(_input_pin, INPUT);
    
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
            _step++;
            break;

        case 1: //init
            ret_val = !digitalRead(_input_pin);
            if (ret_val)
                _step++;
            break;

        case 2:
            on();
            _start = millis();
            _step++;
            break;

        case 3:
            if (get_duration_ms(_start) >= _delay_ms)
                _step++;
            break;

        case 4:
            off();
            _start = millis();
            _step++;
            break;

        case 5:
            if (get_duration_ms(_start) >= _delay_ms)
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
}

void pump::off(void)
{
    if (_on_state)
        digitalWrite(_output_pin, LOW);
    else
        digitalWrite(_output_pin, HIGH); 
}