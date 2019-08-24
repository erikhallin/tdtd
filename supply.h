#ifndef SUPPLY_H
#define SUPPLY_H

#include <stdlib.h>
#include <gl/gl.h>

class supply
{
    public:
        supply(float _pos_x,float _pos_y,float _rotation);

        bool init();
        bool update();
        bool draw();

        float pos_x,pos_y,rotation;
        float rotation_speed;
        bool in_air;
        float lifetime;

    private:
};

#endif // SUPPLY_H
