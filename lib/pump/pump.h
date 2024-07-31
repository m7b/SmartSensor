#ifndef PUMP_H
#define PUMP_H

#include <rws_common.h>

class pump
{
    public:
        pump(uint8_t output_pin, uint8_t input_pin, bool on_state, uint32_t delay_ms);
        ~pump();

        void setup(void);
        void loop(void);

        void set_on_demand(void);
        void set_off_demand(void);

        bool is_on(void);
        bool is_off(void);

        void setCallback_on(std::function<void(void)> callback_on);
        void setCallback_off(std::function<void(void)> callback_off);

    private:
        std::function<void(void)> callback_on;
        std::function<void(void)> callback_off;

        uint8_t _output_pin;
        uint8_t _input_pin;
        bool    _on_state;

        bool _setup;

        bool _on_demand;
        bool _off_demand;

        bool _is_on;
        bool _is_off;

        uint16_t _step;

        uint32_t _delay_ms;
        uint32_t _start;
        uint32_t _start_entprell;

        bool conditions_ok(void);
        void on(void);
        void off(void);
};

#endif // PUMP_H