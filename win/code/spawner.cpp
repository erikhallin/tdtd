#include "spawner.h"

#include "definitions.h"

spawner::spawner(int _pos_x,int _pos_y)
{
    spawn_cooldown=0;
    spawn_delay=_unit_spawn_delay;
    lifetime=_unit_spawner_lifetime;

    pos_x=_pos_x;
    pos_y=_pos_y;
}

bool spawner::update()
{
    if(spawn_cooldown>0) spawn_cooldown-=_time_dif;
    if(lifetime>0) lifetime-=_time_dif;

    if(spawn_cooldown<=0) return true;

    return false;
}

bool spawner::draw()
{

    return true;
}
