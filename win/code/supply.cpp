#include "supply.h"

#include "definitions.h"

supply::supply(float _pos_x,float _pos_y,float _rotation)
{
    pos_x=_pos_x;
    pos_y=_pos_y;
    rotation=_rotation;
    in_air=false;
    rotation_speed=float(rand()%100-50)*0.03;
    lifetime=_supply_lifetime;
}

bool supply::init()
{
    return true;
}

bool supply::update()
{
    if(in_air)
    {
        rotation+=rotation_speed;
    }
    else
    {
        lifetime-=_time_dif;
        if(lifetime<=0) return false;
    }

    return true;
}

bool supply::draw()
{
    int box_size=3;

    float blend=1;
    if(lifetime<3 && !in_air)
    {
        blend=lifetime/3.0;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1,0.5,0,blend);
    glPushMatrix();
    glTranslatef(pos_x,pos_y,0);
    glRotatef(rotation,0,0,1);
    glBegin(GL_QUADS);
    glVertex2f(-box_size,-box_size);
    glVertex2f(-box_size,box_size);
    glVertex2f(box_size,box_size);
    glVertex2f(box_size,-box_size);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glColor4f(0.2,0.2,0.2,blend);
    glVertex2f(-box_size,-box_size);
    glVertex2f(-box_size,box_size);
    glVertex2f(box_size,box_size);
    glVertex2f(box_size,-box_size);
    glVertex2f(-box_size,-box_size);
    glVertex2f(box_size,box_size);
    glEnd();
    glPopMatrix();
    glDisable(GL_BLEND);

    return true;
}
