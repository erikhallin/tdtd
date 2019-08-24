#include "projectile.h"

#include "definitions.h"

projectile::projectile(float _pos_x,float _pos_y, float _rotation)
{
    speed=_projectile_speed;
    pos_x_last=pos_x=_pos_x;
    pos_y_last=pos_y=_pos_y;
    rotation=_rotation;
}

bool projectile::update()
{
    pos_x_last=pos_x;
    pos_y_last=pos_y;

    pos_x+=cosf(rotation*_deg2rad)*speed;
    pos_y+=sinf(rotation*_deg2rad)*speed;

    return true;
}

bool projectile::draw()
{
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(pos_x,pos_y,0);
    //glRotatef(rotation,0,0,1);
    glBegin(GL_POINTS);
    glVertex2f(0,0);
    glEnd();
    glPopMatrix();

    return true;
}
