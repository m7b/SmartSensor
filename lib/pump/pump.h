#ifndef PUMP_H
#define PUMP_H

#include <rws_common.h>
#include <statemachine.h>

class pump : public statemachine
{
    public:
        pump(uint8_t output_pin, bool on_state, uint32_t delay_ms);
        ~pump();

        void setup(void);
        void loop(void);

        void set_on_demand(void);
        void set_ls_on_demand(void);
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
        bool _ls_on_demand;
        bool _off_demand;

        bool _is_on;
        bool _is_off;

        uint32_t _delay_ms;
        uint32_t _start;

        bool conditions_ok(void);
        void on(void);
        void off(void);

        uint32_t _libre_strike_count;
        uint32_t _libre_strike_count_to_perform;
        void libre_strike_on(void);
        void libre_strike_off(void);
};


STEP_DEF(N000_PUMP_INIT_STEP,                "N000: Init step");
STEP_DEF(N010_PUMP_WAIT_FOR_DEMAND_STEP,     "N010: Wait for demand");
STEP_DEF(N020_PUMP_SWITCH_ON_STEP,           "N020: Switch on pump");
STEP_DEF(N030_PUMP_WAIT_FOR_OFF_DEMAND_STEP, "N030: Wait for off demand");
STEP_DEF(N040_PUMP_SWITCH_OFF_STEP,          "N040: Switch off pump");

STEP_DEF(N200_PUMP_INIT_LIBERATION_STRIKE_DEMAND_STEP,  "N200: Init liberation strike demand");
STEP_DEF(N210_PUMP_LIBERATION_STRIKE_ON_STEP,           "N210: Liberation strike on");
STEP_DEF(N220_PUMP_LIBERATION_STRIKE_ON_WAIT_STEP,      "N220: Liberation strike wait after on");
STEP_DEF(N230_PUMP_LIBERATION_STRIKE_OFF_STEP,          "N230: Liberation strike off");
STEP_DEF(N240_PUMP_LIBERATION_STRIKE_OFF_WAIT_STEP,     "N240: Liberation strike wait after off");
STEP_DEF(N250_PUMP_LIBERATION_STRIKE_CHECK_REPEAT_STEP, "N250: Liberation strike for check repeat");
STEP_DEF(N260_PUMP_LIBERATION_STRIKE_FINALIZE_STEP,     "N260: Liberation strike finalize");

STEP_DEF(N999_PUMP_END,                      "N999: End");

#endif // PUMP_H