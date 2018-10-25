#ifndef SMARTSENSOR_BARREL_H
#define SMARTSENSOR_BARREL_H

#include <NewPing.h>


class smartsensor_barrel : public NewPing
{
    public:
        smartsensor_barrel();
        smartsensor_barrel(unsigned long max_fill_level_cm);
        ~smartsensor_barrel();

        unsigned long get_fill_level_cm(void);

    private:
        unsigned long max_fill_level_cm;
};

#endif // SMARTSENSOR_BARREL_H