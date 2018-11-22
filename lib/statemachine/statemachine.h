#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <cstring>

#define MAX_DESCR_LENGTH 80

class statemachine
{
    public:
        statemachine();
        ~statemachine();

        void set_next_step(int step, const char *descr);
        int get_step(void);

    private:
        int _step;
        char _descr[MAX_DESCR_LENGTH];
};


#endif // STATEMACHINE_H