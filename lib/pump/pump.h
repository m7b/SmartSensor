#ifndef PUMP_H
#define PUMP_H

#include <rws_common.h>

class pump
{
    public:
        pump(uint8_t output_pin, uint8_t input_pin);
        ~pump();

        void setup(void);
        void loop(void);

    private:
        uint8_t _output_pin;
        uint8_t _input_pin;

        bool _setup;
        uint16_t _step;

        uint32_t _delay_ms;
        uint32_t _start;

        bool conditions_ok(void);
};

#endif // PUMP_H