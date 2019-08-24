#ifndef BIG_PIXEL_H
#define BIG_PIXEL_H

#include <gl/gl.h>
#include <cmath>

enum enemy_types
{
    et_hammer=0,
    et_rammer
};

class big_pixel
{
    public:
        big_pixel(int _type,float _pos_x,float _pos_y);

        bool init();
        bool update();
        bool draw();
        bool reset_target();

        int type;
        float pos_x,pos_y;
        float pos_x_last,pos_y_last;
        float target_pos_x,target_pos_y;
        float speed;
        float radius;
        float search_cooldown;
        bool  on_mission;

    private:
};

#endif // BIG_PIXEL_H
