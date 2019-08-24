#ifndef SHIP_H
#define SHIP_H

#include <gl/gl.h>
#include <cmath>



using namespace std;

class ship
{
    public:
        ship();

        bool init();
        bool update();
        bool draw();

        float pos_x,pos_y;
        float rotation;
        float speed;
        float speed_mod;
        bool landing,supply_onboard;

        float fire_cooldown;

    private:
};

#endif // SHIP_H
