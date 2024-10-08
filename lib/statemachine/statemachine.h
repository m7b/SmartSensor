#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <rws_common.h>

#define MAX_DESCR_LENGTH 80

//Marco for defining steps
//constexpr statemachine::step N000_INIT_STEP { __COUNTER__ , "N000: Init step"};
#define STEP_DEF(x, y) constexpr statemachine::step x {__COUNTER__, y}

class statemachine
{
    public:
        struct step
        {
            unsigned long id;
            char descr[MAX_DESCR_LENGTH];
            constexpr operator unsigned long() const { return id; }
        };

        statemachine(step init_step);
        ~statemachine();

        void set_next_step(const step &next);
        step get_step(void);
        unsigned long get_step_start_ms(void);
        void print_step_info(const step &s);

    private:
        step _step;
        unsigned long _step_start_ms;
};


#endif // STATEMACHINE_H