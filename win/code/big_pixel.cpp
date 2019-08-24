#include "big_pixel.h"

#include "definitions.h"

big_pixel::big_pixel(int _type,float _pos_x,float _pos_y)
{
    type=_type;
    pos_x_last=target_pos_x=pos_x=_pos_x;
    pos_y_last=target_pos_y=pos_y=_pos_y;
    speed=1;
    search_cooldown=0;
    on_mission=false;
    switch(type)
    {
        case et_hammer: speed=_hammer_speed;
                        radius=3;
                        break;
        case et_rammer: speed=_rammer_speed;
                        radius=5;
                        break;
    }
}

bool big_pixel::init()
{

    return true;
}

bool big_pixel::update()
{
    if(search_cooldown>0) search_cooldown-=_time_dif;

    if(pos_x+radius>target_pos_x && pos_x-radius<target_pos_x &&
       pos_y+radius>target_pos_y && pos_y-radius<target_pos_y )
    {
        //target found, make new target
        return true;
    }

    pos_x_last=pos_x;
    pos_y_last=pos_y;

    //move towards target
    float dir_x=target_pos_x-pos_x;
    float dir_y=target_pos_y-pos_y;
    float length=sqrt( dir_x*dir_x+dir_y*dir_y );
    dir_x/=length;
    dir_y/=length;

    //update pos
    pos_x+=dir_x*speed;
    pos_y+=dir_y*speed;

    return false;
}

bool big_pixel::draw()
{
    glColor3f(1,1,1);
    glPointSize((int)radius);
    glBegin(GL_POINTS);
    glVertex2f(pos_x,pos_y);
    glEnd();

    return true;
}

bool big_pixel::reset_target()
{
    target_pos_x=pos_x=pos_x_last;
    target_pos_y=pos_y=pos_y_last;
    on_mission=false;

    return true;
}
