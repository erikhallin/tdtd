#include "homebase.h"

#include "definitions.h"

homebase::homebase()
{
    //ctor
}

homebase::homebase(int _pos_x,int _pos_y)
{
    pos_x=_pos_x;
    pos_y=_pos_y;
    click_flag=true;
}

bool homebase::landtest(int _pos_x,int _pos_y,float _rotation)
{
    float rotation_error_tol=10;
    int land_pos_size=5;

    //test left site
    if(_rotation-rotation_error_tol<180 && _rotation+rotation_error_tol>180)
    {
        //check pos
        if(_pos_x+land_pos_size>pos_x+15 && _pos_x-land_pos_size<pos_x+15 &&
           _pos_y+land_pos_size>pos_y && _pos_y-land_pos_size<pos_y)
        {
            //within zone, land
            return true;
        }
    }

    //test right site
    if(_rotation<rotation_error_tol || _rotation>360-rotation_error_tol)
    {
        //check pos
        if(_pos_x+land_pos_size>pos_x-15 && _pos_x-land_pos_size<pos_x-15 &&
           _pos_y+land_pos_size>pos_y && _pos_y-land_pos_size<pos_y)
        {
            //within zone, land
            return true;
        }
    }

    return false;
}

bool homebase::draw()
{
    int width=_homebase_width;
    int height=_homebase_height;

    glColor3f(0.2,0.2,0.2);
    glBegin(GL_QUADS);
    glVertex2f(pos_x-width*0.4,pos_y+height+7);
    glVertex2f(pos_x-width*0.4,pos_y-height-7);
    glColor3f(0.2,0.2,0.2);
    glVertex2f(pos_x+width*0.4,pos_y-height-7);
    glVertex2f(pos_x+width*0.4,pos_y+height+7);
    glEnd();

    if(click_flag)
    {
        click_flag=false;

        glColor3f(0.5,0.5,0.5);
        glBegin(GL_QUADS);
        glVertex2f(pos_x-width*0.4,pos_y+height+7);
        glVertex2f(pos_x-width*0.4,pos_y-height-7);
        glColor3f(0.5,0.5,0.5);
        glVertex2f(pos_x+width*0.4,pos_y-height-7);
        glVertex2f(pos_x+width*0.4,pos_y+height+7);
        glEnd();
    }

    glColor3f(0.1,0.1,0.1);
    glBegin(GL_LINE_STRIP);
    glVertex2f(pos_x-width*0.4,pos_y+height+7);
    glVertex2f(pos_x-width*0.4,pos_y-height-7);
    glVertex2f(pos_x+width*0.4,pos_y-height-7);
    glVertex2f(pos_x+width*0.4,pos_y+height+7);
    glVertex2f(pos_x-width*0.4,pos_y+height+7);
    glEnd();

    glColor3f(0.3,0.3,0.3);
    glBegin(GL_QUADS);
    glVertex2f(pos_x-width*1.0,pos_y+height);
    glVertex2f(pos_x-width*1.0,pos_y-height);
    glColor3f(0.4,0.4,0.4);
    glVertex2f(pos_x-width*0.3,pos_y-height);
    glVertex2f(pos_x-width*0.3,pos_y+height);
    glVertex2f(pos_x-width*0.3,pos_y+height);
    glVertex2f(pos_x-width*0.3,pos_y-height);
    glVertex2f(pos_x+width*0.3,pos_y-height);
    glVertex2f(pos_x+width*0.3,pos_y+height);
    glVertex2f(pos_x+width*0.3,pos_y+height);
    glVertex2f(pos_x+width*0.3,pos_y-height);
    glColor3f(0.3,0.3,0.3);
    glVertex2f(pos_x+width*1.0,pos_y-height);
    glVertex2f(pos_x+width*1.0,pos_y+height);
    glEnd();

    glColor3f(0.9,0.9,0.9);
    glBegin(GL_LINES);
    glVertex2f(pos_x-18,pos_y);
    glVertex2f(pos_x-15,pos_y);
    glVertex2f(pos_x-11,pos_y);
    glVertex2f(pos_x-8,pos_y);
    glVertex2f(pos_x-4,pos_y);
    glVertex2f(pos_x-2,pos_y);
    glVertex2f(pos_x+2,pos_y);
    glVertex2f(pos_x+4,pos_y);
    glVertex2f(pos_x+8,pos_y);
    glVertex2f(pos_x+11,pos_y);
    glVertex2f(pos_x+15,pos_y);
    glVertex2f(pos_x+18,pos_y);
    glEnd();

    return true;
}
