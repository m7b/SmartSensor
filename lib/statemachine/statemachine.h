#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <rws_common.h>
#include <cstring>

#define MAX_DESCR_LENGTH 80

struct step
{
    unsigned long id;
    char descr[MAX_DESCR_LENGTH];

    static unsigned long gen_id()
    {
        static unsigned long i = 0;
        return ++i;
    }

    constexpr operator unsigned long() const { return id; }
    
};


class statemachine
{
    public:
        statemachine();
        ~statemachine();

        void set_next_step(step);
        unsigned long get_step(void);

    private:
        step _step;
        char _descr[MAX_DESCR_LENGTH];
        unsigned long _step_start;
};


#endif // STATEMACHINE_H