#ifndef RGBLED_H
#define RGBLED_H

#include <Arduino.h>

class rgbled
{
    public:
        rgbled();
        rgbled(uint8_t red, uint8_t green, uint8_t blue);
        ~rgbled();

        void setup(void);
        void loop(void);

        void set_rgb_port(uint8_t red, uint8_t green, uint8_t blue);
        void set_delay_ms(uint32_t delay_ms);

    private:
        bool _setup;
        uint16_t _step;

        uint8_t _red;
        uint8_t _green;
        uint8_t _blue;

        uint8_t _act_led;

        uint32_t _delay_ms;
        uint32_t _start;

        bool conditions_ok(void);
        uint32_t get_duration(uint32_t start);
};

#endif // RGBLED_H