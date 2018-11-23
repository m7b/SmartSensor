#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <rws_common.h>
#include <cstring>

#define MAX_DESCR_LENGTH 80

//Marco for defining steps
//constexpr step N000_INIT_STEP { __COUNTER__ , "N000: Init step"};
#define STEP_DEF(x, y) constexpr step x {__COUNTER__, y}

struct step
{
    unsigned long id;
    char descr[MAX_DESCR_LENGTH];
    //step(const step& a) : id(a.id) { } // user-defined copy ctor
    constexpr operator unsigned long() const { return id; }
};



class statemachine
{
    public:
        statemachine();
        ~statemachine();

        void set_next_step(const step &next);
        unsigned long get_step(void);

    private:
        step _step;
        char _descr[MAX_DESCR_LENGTH];
        unsigned long _step_start;
};


#endif // STATEMACHINE_H