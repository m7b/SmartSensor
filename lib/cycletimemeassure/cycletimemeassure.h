#ifndef CYCLETIMEMEASSURE_H
#define CYCLETIMEMEASSURE_H

#include <rws_common.h>
#include <iomanip>
#include <sstream>

class cycletimemeassure
{
    public:
        cycletimemeassure(int buffer_size);
        ~cycletimemeassure();

        void take_meassure_point(void);
        float get_cycle_time(void);
        std::string get_cycle_time(unsigned int precision);

    private:
        int _buffer_size;
        int _i;
        unsigned long _cycle_time;
        unsigned long *_buffer;
        unsigned long _tmp1, _tmp2;
        float _ct;
        bool _calculated;

        void calc_ct(void);

};


#endif // CYCLETIMEMEASSURE_H