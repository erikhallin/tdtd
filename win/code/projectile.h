#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <gl/gl.h>
#include <cmath>

class projectile
{
    public:
        projectile(float pos_x,float pos_y, float rotation);

        float pos_x,pos_y;
        float rotation;
        float speed;
        float pos_x_last,pos_y_last;

        bool update();
        bool draw();

    private:
};

#endif // PROJECTILE_H
