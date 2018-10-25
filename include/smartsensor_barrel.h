#ifndef SMARTSENSOR_BARREL_H
#define SMARTSENSOR_BARREL_H

#include <NewPing.h>

#define TRIGGER_PIN         0   //Trigger-Pin
#define ECHO_PIN            1   //Echo-Pin
#define MAX_DISTANCE      200   //Max distance in [cm]
#define MAX_FILL_LEVEL_CM 110   //Max fill level of barrel in [cm]

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