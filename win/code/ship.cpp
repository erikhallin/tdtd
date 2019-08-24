#include "ship.h"

#include "definitions.h"

ship::ship()
{
    //ctor
}

bool ship::init()
{
    pos_x=300;
    pos_y=300;
    rotation=0;
    speed=_ship_speed;
    speed_mod=1.0;
    fire_cooldown=0;
    landing=false;
    supply_onboard=false;

    return true;
}

bool ship::update()
{
    pos_x+=cosf(rotation*_deg2rad)*speed*speed_mod;
    pos_y+=sinf(rotation*_deg2rad)*speed*speed_mod;

    if(fire_cooldown>0) fire_cooldown-=_time_dif;

    //landing progress
    if(landing && speed>0)
    {
        speed-=_time_dif*2.5;
        if(speed<0) speed=0;
    }
    //takeoff progress
    if(!landing && speed<_ship_speed)
    {
        speed+=_time_dif*3;
        if(speed>_ship_speed) speed=_ship_speed;
    }

    return true;
}

bool ship::draw()
{

    glPushMatrix();
    glTranslatef(pos_x,pos_y,0);
    glRotatef(rotation,0,0,1);
    glBegin(GL_TRIANGLES);
    glColor3f(0.8,0.8,0.8);
    glVertex2f(-5,3);
    glVertex2f(-5,-3);
    glColor3f(0.9,0.9,0.9);
    glVertex2f(5,0);
    glEnd();
    glBegin(GL_LINES);
    glColor3f(0.5,0.5,0.5);
    glVertex2f(-7,0);
    glVertex2f(5,0);
    glEnd();
    glPopMatrix();

    return true;
}
