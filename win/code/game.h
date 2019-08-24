#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <SOIL/SOIL.h>
#include <gl/gl.h>
#include <vector>
#include <string>
#include <ctime>
#include <stdlib.h>
#include <sstream>

#include "ship.h"
#include "projectile.h"
#include "spawner.h"
#include "homebase.h"
#include "supply.h"
#include "definitions.h"
#include "sound.h"
#include "perlinnoise.h"
#include "big_pixel.h"

using namespace std;

enum objects_on_space
{
    oos_empty=0,
    oos_unit,
    oos_wall,
    oos_tower,
    oos_na
};

struct grid_event
{
    grid_event()
    {
        oos=oos_empty;
        already_updated=false;
        wall_hp=_wall_hp;
        //r=g=b=float(rand()%100)*0.01*0.1;
    }
    grid_event(int _oos)
    {
        oos=_oos;
        already_updated=false;
        wall_hp=_wall_hp;
        //r=g=b=float(rand()%100)*0.01*0.1;
        //g+=0.02;
    }
    int oos;
    int wall_hp;
    bool already_updated;
    float r,g,b;
};

struct ray
{
    ray(int x1,int y1,int x2,int y2)
    {
        x_1=x1;
        y_1=y1;
        x_2=x2;
        y_2=y2;
    }

    int x_1,y_1;
    int x_2,y_2;
};

enum gamestates
{
    gs_init=0,
    gs_main,
    gs_info,
    gs_run,
    gs_gameover
};

class game
{
    public:
        game();

        int game_state;

        bool init(int screen_width,int screen_height);
        bool update();
        bool draw();
        bool reset();
        bool draw_number(char val);

        int mouse_pos[2];
        int mouse_pos_last[2];
        bool mouse_but[4];
        bool rmb_pressed;
        bool lmb_pressed;
        bool w_pressed,s_pressed;

        bool keys[256];

        vector<projectile> vec_projectiles;
        vector<spawner> vec_spawners;
        vector<ray> vec_rays;
        vector<supply> vec_supplies;
        vector<big_pixel> vec_bigpixels;

        //sound
        sound* pSound;
        //texture
        int tex_main,tex_info,tex_gameover;

    private:

        int screen_width, screen_height;
        grid_event** grid;
        ship Ship;
        homebase Homebase;
        float unit_move_cooldown;
        float tower_fire_cooldown;
        float supply_spawn_cooldown;
        float unit_spawner_cooldown;
        float supply_meter;
        float resource_meter;
        float game_timer;
        int level_counter,level_difficulty_tag;
};

#endif // GAME_H
