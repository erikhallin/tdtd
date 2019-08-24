#include "game.h"

#include "definitions.h"

game::game()
{
    game_state=gs_init;
}

bool game::init(int _screen_width,int _screen_height)
{
    srand(time(0));

    screen_width=_screen_width;
    screen_height=_screen_height;

    unit_move_cooldown=0;
    tower_fire_cooldown=1.9;
    supply_spawn_cooldown=0;
    unit_spawner_cooldown=0;
    resource_meter=200;
    supply_meter=300;
    game_timer=0;
    level_counter=level_difficulty_tag=0;


    PerlinNoise* pNoise=new PerlinNoise(0.9,0.03,1,4,rand()%10000);

    grid=new grid_event*[_screen_width];
    for(int i=0;i< _screen_width;++i) grid[i] = new grid_event[_screen_height];

    for(int x=0;x<_screen_width;x++)
    for(int y=0;y<_screen_height;y++)
    {
        grid[x][y].oos=oos_empty;
        grid[x][y].r=grid[x][y].g=grid[x][y].b=(pNoise->GetHeight(x,y)+0.5)*0.02+float(rand()%100)*0.0004;
        grid[x][y].g+=0.03;
    }

    //input reset
    for(int i=0;i<256;i++) keys[i]=0;
    mouse_but[0]=0;
    mouse_but[1]=0;
    mouse_but[2]=0;
    mouse_but[3]=0;
    mouse_pos_last[0]=mouse_pos_last[1]=mouse_pos[0]=mouse_pos[1]=0;
    rmb_pressed=false;
    lmb_pressed=false;
    w_pressed=s_pressed=false;

    //place bigpixels
    //vec_bigpixels.push_back(big_pixel(et_hammer,200,200));
    //vec_bigpixels.push_back(big_pixel(et_hammer,203,200));
    //vec_bigpixels.push_back(big_pixel(et_hammer,207,200));
    //vec_bigpixels.push_back(big_pixel(et_rammer,200,300));
    //vec_bigpixels.push_back(big_pixel(et_rammer,203,300));
    //vec_bigpixels.push_back(big_pixel(et_rammer,207,300));

    //ship
    Ship.init();
    Ship.pos_x=screen_width*0.2;
    Ship.pos_y=screen_height*0.5;

    //homebase
    Homebase=homebase(screen_width*0.5,screen_height*0.5);

    //place spawner
    vec_spawners.push_back(spawner(100,100));

    //place supply
    vec_supplies.push_back(supply(400,410,17));
    vec_supplies.push_back(supply(420,400,87));
    /*vec_supplies.push_back(supply(440,420,17));
    vec_supplies.push_back(supply(460,400,17));
    vec_supplies.push_back(supply(400,460,17));*/

    //load texture
    tex_main=SOIL_load_OGL_texture
    (
        "data\\texture\\main.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_info=SOIL_load_OGL_texture
    (
        "data\\texture\\info.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
    );
    tex_gameover=SOIL_load_OGL_texture
    (
        "data\\texture\\gameover.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
    );
    if(tex_main==0 || tex_info==0 || tex_gameover==0)
    {
        cout<<"ERROR: Could not load textures\n";
        return false;
    }

    //load sound
    pSound=new sound();
    bool error_flag=false;
    if( !pSound->load_WAVE_from_file( wav_song1,"data\\sound\\song1.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_clicker,"data\\sound\\clicker.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_gameover,"data\\sound\\gameover.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_place_tower,"data\\sound\\place_tower.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_place_wall,"data\\sound\\place_wall.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_ship_fire,"data\\sound\\ship_fire.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_ship_land,"data\\sound\\ship_land.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_ship_takeoff,"data\\sound\\ship_takeoff.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_start,"data\\sound\\start.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_supply_delivery,"data\\sound\\supply_delivery.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_supply_pickup,"data\\sound\\supply_pickup.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_tower_fire,"data\\sound\\tower_fire.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_pixel_hit,"data\\sound\\pixel_hit.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_speed_up,"data\\sound\\speed_up.wav" ) ) error_flag=true;
    if( !pSound->load_WAVE_from_file( wav_speed_down,"data\\sound\\speed_down.wav" ) ) error_flag=true;
    if(error_flag)
    {
        cout<<"ERROR: Could not load sounds\n";
        return false;
    }

    //play music
    pSound->playSimpleSound(wav_song1,1,10,true);

    cout<<"Game init successful\n\n";

    game_state=gs_main;

    return true;
}

bool game::update()
{
    if(game_state==gs_main)
    {
        //check for input to continue
        if(mouse_but[0])
        {
            if(!lmb_pressed)
            {
                lmb_pressed=true;
                game_state=gs_info;
            }
        }
        else lmb_pressed=false;

        return true;
    }

    if(game_state==gs_info)
    {
        //check for input to continue
        if(mouse_but[0])
        {
            if(!lmb_pressed)
            {
                lmb_pressed=true;
                game_state=gs_run;
                game_timer=0;

                pSound->playSimpleSound(wav_start,0.3);

                //first click fix
                mouse_but[0]=false;
            }
        }
        else lmb_pressed=false;

        return true;
    }

    if(game_state==gs_gameover)
    {
        //check for input to continue
        if(mouse_but[0])
        {
            if(!lmb_pressed)
            {
                //reset
                reset();

                lmb_pressed=true;
                game_state=gs_run;

                //first click fix
                mouse_but[0]=false;
            }
        }
        else lmb_pressed=false;

        return true;
    }

    //update time
    game_timer+=_time_dif;

    //update keyboard input
    if(keys[65] && !Ship.landing && Ship.speed>_ship_speed*0.9)//a
    {
        if(keys[17]) Ship.rotation-=_ship_rotation_speed*0.5;
        else         Ship.rotation-=_ship_rotation_speed;
        if(Ship.rotation<0) Ship.rotation+=360;
    }
    if(keys[68] && !Ship.landing && Ship.speed>_ship_speed*0.9)//d
    {
        if(keys[17]) Ship.rotation+=_ship_rotation_speed*0.5;
        else         Ship.rotation+=_ship_rotation_speed;
        if(Ship.rotation>360) Ship.rotation-=360;
    }
    if(keys[32] && Ship.fire_cooldown<=0)//space
    {
        vec_projectiles.push_back( projectile(Ship.pos_x,Ship.pos_y,Ship.rotation) );
        Ship.fire_cooldown+=_ship_fire_cooldown;

        pSound->playSimpleSound(wav_ship_fire,0.1);
    }
    //modify speed
    if(!Ship.landing && Ship.speed>_ship_speed*0.9)
    {
        if(keys[87])//w
        {
            Ship.speed_mod=1.5;

            if(!w_pressed)
            {
                pSound->playSimpleSound(wav_speed_up,0.2);
            }
            w_pressed=true;
        }
        else if(keys[83])//s
        {
            Ship.speed_mod=0.7;

            if(!s_pressed)
            {
                pSound->playSimpleSound(wav_speed_down,0.2);
            }
            s_pressed=true;
        }
        else Ship.speed_mod=1;
    }
    else Ship.speed_mod=1;
    if(!keys[87]) w_pressed=false;
    if(!keys[83]) s_pressed=false;

    //update left mouse input
    if(mouse_but[0] && Ship.speed!=0)
    {
        //is mouse on screen
        if(mouse_pos[0]>=0 && mouse_pos[0]<screen_width &&
           mouse_pos[1]>=0 && mouse_pos[1]<screen_height)
        {
            //check if empty space
            if(grid[mouse_pos[0]][mouse_pos[1]].oos==oos_empty)
            {
                //build tower
                if(resource_meter>=_cost_tower)  //check if resources available
                {
                    grid[mouse_pos[0]][mouse_pos[1]].oos=oos_tower;

                    //consume resources
                    resource_meter-=_cost_tower;

                    pSound->playSimpleSound(wav_place_tower,0.2);
                }
            }

            lmb_pressed=true;
        }
    }
    if(!mouse_but[0]) lmb_pressed=false;

    //update right mouse input
    if(mouse_but[1] && Ship.speed!=0)
    {
        //is mouse on screen
        if(mouse_pos[0]>=0 && mouse_pos[0]<screen_width &&
           mouse_pos[1]>=0 && mouse_pos[1]<screen_height)
        {
            //check if empty space
            if(grid[mouse_pos[0]][mouse_pos[1]].oos==oos_empty)
            {
                //consume resources

                //build wall
                if(resource_meter>=_cost_wall)  //check if resources available
                {
                    grid[mouse_pos[0]][mouse_pos[1]].oos=oos_wall;

                    //consume resources
                    resource_meter-=_cost_wall;

                    pSound->playSimpleSound(wav_place_wall,0.1);
                }


                //if hold, draw wall path
                if(rmb_pressed)
                {
                    //build x-row
                    if(mouse_pos[0]>mouse_pos_last[0])
                    for(int x=mouse_pos_last[0];x<=mouse_pos[0];x++)
                    {
                        if(grid[x][mouse_pos[1]].oos==oos_empty && resource_meter>=_cost_wall)
                        {
                            grid[x][mouse_pos[1]].oos=oos_wall;
                            //consume resources
                            resource_meter-=_cost_wall;
                        }
                    }
                    else
                    for(int x=mouse_pos[0];x<=mouse_pos_last[0];x++)
                    {
                        if(grid[x][mouse_pos[1]].oos==oos_empty && resource_meter>=_cost_wall)
                        {
                            grid[x][mouse_pos[1]].oos=oos_wall;
                            //consume resources
                            resource_meter-=_cost_wall;
                        }
                    }

                    //build y-row
                    if(mouse_pos[1]>mouse_pos_last[1])
                    for(int y=mouse_pos_last[1];y<=mouse_pos[1];y++)
                    {
                        if(grid[mouse_pos_last[0]][y].oos==oos_empty && resource_meter>=_cost_wall)
                        {
                            grid[mouse_pos_last[0]][y].oos=oos_wall;
                            //consume resources
                            resource_meter-=_cost_wall;
                        }
                    }
                    else
                    for(int y=mouse_pos[1];y<=mouse_pos_last[1];y++)
                    {
                        if(grid[mouse_pos_last[0]][y].oos==oos_empty && resource_meter>=_cost_wall)
                        {
                            grid[mouse_pos_last[0]][y].oos=oos_wall;
                            //consume resources
                            resource_meter-=_cost_wall;
                        }
                    }
                }
                else
                {
                    rmb_pressed=true;
                }

                //remember last pos
                mouse_pos_last[0]=mouse_pos[0];
                mouse_pos_last[1]=mouse_pos[1];
            }
        }


    }
    if(!mouse_but[1]) rmb_pressed=false;

    //clicker mode
    if(mouse_but[0] && Ship.speed==0)
    {
        if(!lmb_pressed)
        {
            if(supply_meter>=_supply_convert_speed)
            {
                supply_meter-=_supply_convert_speed;
                resource_meter+=_supply_convert_speed;

                pSound->playSimpleSound(wav_clicker,0.2);

                //activate homebase draw
                Homebase.click_flag=true;
            }
            lmb_pressed=true;
        }
    }

    //spawn special units based on difficulty
    if(level_counter>level_difficulty_tag)
    {
        level_difficulty_tag++;
        //pick side of entrance
        int side=rand()%4;//up,left,down,right
        switch(level_counter)
        {
            case 1:
            {
                for(int i=0;i<1;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<0;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;

            case 2:
            {
                for(int i=0;i<1;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<0;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;

            case 3:
            {
                for(int i=0;i<0;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<1;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;

            case 4:
            {
                for(int i=0;i<1;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<1;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;

            case 5:
            {
                for(int i=0;i<2;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<2;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;

            case 6:
            {
                for(int i=0;i<0;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<6;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;

            case 7:
            {
                for(int i=0;i<5;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<1;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;

            case 8:
            {
                for(int i=0;i<10;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<1;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;

            case 9:
            {
                for(int i=0;i<0;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<10;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;

            default:
            {
                for(int i=0;i<level_counter;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_hammer,x,y));
                }
                for(int i=0;i<level_counter;i++)
                {
                    int x=rand()%screen_width;
                    int y=rand()%screen_height;
                    switch(side)
                    {
                        case 0: y=0; break;
                        case 1: x=0; break;
                        case 2: y=screen_height-1; break;
                        case 3: x=screen_width-1; break;
                    }
                    vec_bigpixels.push_back(big_pixel(et_rammer,x,y));
                }
            }break;
        }
    }

    //update big pixels
    for(int i=0;i<(int)vec_bigpixels.size();i++)
    {
        if(vec_bigpixels[i].update())
        {
            //target reached, give new target
            switch(vec_bigpixels[i].type)
            {
                case et_hammer:
                {
                    //random pos
                    vec_bigpixels[i].target_pos_x=rand()%screen_width;
                    vec_bigpixels[i].target_pos_y=rand()%screen_height;
                }break;

                case et_rammer:
                {
                    //go straight only
                    if(rand()%2==0)
                    {
                        //x travel
                        vec_bigpixels[i].target_pos_x=rand()%screen_width;
                    }
                    else
                    {
                        //y-travel
                        vec_bigpixels[i].target_pos_y=rand()%screen_height;
                    }
                }break;
            }
        }

        //check if towers nearby hammers
        if(vec_bigpixels[i].type==et_hammer)
        {
            //check if mission is active
            if(vec_bigpixels[i].on_mission)
            {
                if(vec_bigpixels[i].target_pos_x>=0 && vec_bigpixels[i].target_pos_x<screen_width &&
                   vec_bigpixels[i].target_pos_y>=0 && vec_bigpixels[i].target_pos_y<screen_height)
                if(grid[(int)vec_bigpixels[i].target_pos_x][(int)vec_bigpixels[i].target_pos_y].oos!=oos_tower)
                {
                    //cancel mission
                    vec_bigpixels[i].on_mission=false;
                }
            }

            if(vec_bigpixels[i].search_cooldown<=0 && !vec_bigpixels[i].on_mission)
            {
                vec_bigpixels[i].search_cooldown=_hammer_search_delay;
                //grid search
                for(int x=(int)vec_bigpixels[i].pos_x-_hammer_search_radius;
                       x<=(int)vec_bigpixels[i].pos_x+_hammer_search_radius && x<screen_width;
                       x++)
                for(int y=(int)vec_bigpixels[i].pos_y-_hammer_search_radius;
                       y<=(int)vec_bigpixels[i].pos_y+_hammer_search_radius && y<screen_height;
                       y++)
                {
                    if(x<0 || y<0) continue;
                    if(grid[x][y].oos==oos_tower)
                    {
                        //set target
                        vec_bigpixels[i].target_pos_x=x;
                        vec_bigpixels[i].target_pos_y=y;
                        vec_bigpixels[i].on_mission=true;
                    }
                }
            }
        }

        //test if pos is ok
        if(vec_bigpixels[i].pos_x<0 || vec_bigpixels[i].pos_x>=screen_width ||
           vec_bigpixels[i].pos_y<0 || vec_bigpixels[i].pos_y>=screen_height)
        {
            vec_bigpixels[i].reset_target();
            continue;
        }
        if(grid[(int)vec_bigpixels[i].pos_x][(int)vec_bigpixels[i].pos_y].oos==oos_wall)
        {
            if(vec_bigpixels[i].type==et_hammer)//rammer is immune
            {
                vec_bigpixels[i].reset_target();
                continue;
            }
        }

        //check destruction
        for(int x=(int)vec_bigpixels[i].pos_x-(int)vec_bigpixels[i].radius;
            x<=(int)vec_bigpixels[i].pos_x+(int)vec_bigpixels[i].radius && x<screen_width;
            x++)
        for(int y=(int)vec_bigpixels[i].pos_y-(int)vec_bigpixels[i].radius;
            y<=(int)vec_bigpixels[i].pos_y+(int)vec_bigpixels[i].radius && y<screen_height;
            y++)
        {
            if(x<0 || y<0) continue;
            //check if towers nearby
            if(grid[x][y].oos==oos_tower)
            {
                grid[x][y].oos=oos_empty;

                //allow new mission search for hammers
                if(vec_bigpixels[i].type==et_hammer)
                {
                    vec_bigpixels[i].on_mission=false;
                    vec_bigpixels[i].search_cooldown=0;
                }
            }
            else if(grid[x][y].oos==oos_wall && vec_bigpixels[i].type==et_rammer)
            {
                //rammer destroy walls
                grid[x][y].oos=oos_empty;
            }
        }
    }

    //update projectiles
    for(int i=0;i<(int)vec_projectiles.size();i++)
    {
        vec_projectiles[i].update();

        if(vec_projectiles[i].pos_x<0 || vec_projectiles[i].pos_x>=screen_width ||
           vec_projectiles[i].pos_y<0 || vec_projectiles[i].pos_y>=screen_height)
        {
            //outside world, remove
            vec_projectiles.erase(vec_projectiles.begin()+i);
            i--;
            continue;
        }

        //check collision with units
        //check all points between last pos and the current
        //get direction
        float dir_x=vec_projectiles[i].pos_x-vec_projectiles[i].pos_x_last;
        float dir_y=vec_projectiles[i].pos_y-vec_projectiles[i].pos_y_last;
        float length=sqrt( dir_x*dir_x+dir_y*dir_y );
        dir_x/=length;
        dir_y/=length;
        bool projectile_removed=false;
        for(int len=0;len<=(int)length;len++)
        {
            if(projectile_removed) break;
            int x=int(vec_projectiles[i].pos_x_last+dir_x*len);
            int y=int(vec_projectiles[i].pos_y_last+dir_y*len);
            if(x<0 || x>=screen_width || y<0 || y>=screen_height) continue;

            //test collision
            if(grid[x][y].oos==oos_unit)
            {
                //remove that unit
                grid[x][y].oos=oos_empty;

                //remove projectile
                vec_projectiles.erase(vec_projectiles.begin()+i);
                i--;
                break;
            }
            else //test big pixels
            {
                for(int bp=0;bp<(int)vec_bigpixels.size();bp++)
                {
                    if(vec_bigpixels[bp].pos_x+vec_bigpixels[bp].radius>x && vec_bigpixels[bp].pos_x-vec_bigpixels[bp].radius<x &&
                       vec_bigpixels[bp].pos_y+vec_bigpixels[bp].radius>y && vec_bigpixels[bp].pos_y-vec_bigpixels[bp].radius<y)
                    {
                        projectile_removed=true;

                        //remove projectile
                        vec_projectiles.erase(vec_projectiles.begin()+i);
                        i--;

                        //remove big pixel
                        vec_bigpixels.erase(vec_bigpixels.begin()+bp);

                        pSound->playSimpleSound(wav_pixel_hit,0.2);

                        break;
                    }
                }
            }
        }


    }

    //update unit spawners
    for(int i=0;i<(int)vec_spawners.size();i++)
    {
        if(vec_spawners[i].update())
        {
            //spawn unit
            vec_spawners[i].spawn_cooldown=vec_spawners[i].spawn_delay;
            if(grid[vec_spawners[i].pos_x][vec_spawners[i].pos_y].oos==oos_empty)
                grid[vec_spawners[i].pos_x][vec_spawners[i].pos_y]=oos_unit;

            //check lifetime
            if(vec_spawners[i].lifetime<=0)
            {
                vec_spawners.erase(vec_spawners.begin()+i);
                i--;
                continue;
            }
        }
    }

    //update ship
    Ship.update();

    //spawn supply
    if(supply_spawn_cooldown<=0)
    {
        supply_spawn_cooldown=_supply_spawn_delay;
        float x=rand()%screen_width;
        float y=rand()%screen_height;
        float rot=rand()%360;
        vec_supplies.push_back(supply(x,y,rot));
    }
    else supply_spawn_cooldown-=_time_dif;

    //unit spawner spawn
    if(unit_spawner_cooldown<=0)
    {
        unit_spawner_cooldown=_unit_spawner_delay-level_counter;
        if(unit_spawner_cooldown<2) unit_spawner_cooldown=2;
        int x=rand()%screen_width;
        int y=rand()%screen_height;
        int avoid_range=100;
        if(x+avoid_range>screen_width*0.5 && x-avoid_range<screen_width*0.5)
        {
            if(x>screen_width*0.5)
                x+=avoid_range;
            else
                x-=avoid_range;
        }
        if(y+avoid_range>screen_height*0.5 && y-avoid_range<screen_height*0.5)
        {
            if(y>screen_width*0.5)
                y+=avoid_range;
            else
                y-=avoid_range;
        }
        if(x<1) x=1;
        if(y<1) y=1;
        if(x>=screen_width-2) x=screen_width-2;
        if(y>=screen_height-2) y=screen_height-2;

        //move to border
        switch(rand()%4)
        {
            case 0: x=1; break;
            case 1: y=1; break;
            case 2: x=screen_width-2; break;
            case 3: y=screen_height-2; break;
        }

        /*//move to border
        if(x<screen_width-x)
        {
            //left rather than right
            if(y<screen_height-y)
            {
                //up rather than down
                if(x<y)
                {
                    //move left
                    x=1;
                }
                else
                {
                    //move up
                    y=1;
                }
            }
            else
            {
                //down rather than up
                if(x<screen_height-y)
                {
                    //move left
                    x=1;
                }
                else
                {
                    //move down
                    y=screen_height-2;
                }
            }
        }
        else
        {
            //right rather than left
            if(y<screen_height-y)
            {
                //up rather than down
                if(screen_width-x<y)
                {
                    //move right
                    x=screen_width-2;
                }
                else
                {
                    //move up
                    y=1;
                }
            }
            else
            {
                //down rather than up
                if(screen_width-x<screen_height-y)
                {
                    //move right
                    x=screen_width-2;
                }
                else
                {
                    //move down
                    y=screen_height-2;
                }
            }
        }*/

        vec_spawners.push_back(spawner(x,y));
    }
    else unit_spawner_cooldown-=_time_dif;

    //update supply boxes
    int box_counter=0;
    for(int i=0;i<(int)vec_supplies.size();i++)
    {
        //check collision with ship
        int hitbox_size=5;
        if(vec_supplies[i].pos_x+hitbox_size>Ship.pos_x && vec_supplies[i].pos_x-hitbox_size<Ship.pos_x &&
           vec_supplies[i].pos_y+hitbox_size>Ship.pos_y && vec_supplies[i].pos_y-hitbox_size<Ship.pos_y)
        {
            if(!vec_supplies[i].in_air)
                pSound->playSimpleSound(wav_supply_pickup,0.2);

            vec_supplies[i].in_air=true;
        }

        //move boxes in air to the ship
        if(vec_supplies[i].in_air)
        {
            box_counter++;
            float line_length=5+4*box_counter*Ship.speed;
            vec_supplies[i].pos_x=Ship.pos_x-cosf(Ship.rotation*_deg2rad)*line_length;
            vec_supplies[i].pos_y=Ship.pos_y-sinf(Ship.rotation*_deg2rad)*line_length;
        }

        if(!vec_supplies[i].update())
        {
            //remove, end of life
            vec_supplies.erase(vec_supplies.begin()+i);
            i--;
        }
    }

    //update units
    if(unit_move_cooldown<=0)//move all units
    {
        unit_move_cooldown=_unit_move_delay;
        for(int x=0;x<screen_width;x++)
        for(int y=0;y<screen_height;y++)
        {
            if(grid[x][y].oos==oos_unit && !grid[x][y].already_updated)
            {
                //test if at the homebase, gameover
                if(x-_homebase_width<Homebase.pos_x && x+_homebase_width>Homebase.pos_x &&
                   y-_homebase_height<Homebase.pos_y && y+_homebase_height>Homebase.pos_y)
                {
                    game_state=gs_gameover;
                    pSound->stopSound(10);
                    pSound->playSimpleSound(wav_gameover,0.7);
                }

                //move
                int direction=rand()%4;//up,left,down,right
                int error_tol=20;
                if(rand()%2==0) error_tol=40;
                float move_chance=0.4;
                if(direction==0 && y>Homebase.pos_y+error_tol) move_chance+=0.4;
                if(direction==2 && y<Homebase.pos_y-error_tol) move_chance+=0.4;
                if(direction==1 && x>Homebase.pos_x+error_tol) move_chance+=0.4;
                if(direction==3 && x<Homebase.pos_x-error_tol) move_chance+=0.4;

                //decide if move
                if(float(rand()%100)*0.01>move_chance) continue;//no movement

                int pos_x_new=x;
                int pos_y_new=y;

                float long_walk=1;
                if(_enable_longwalk && level_counter>10 && rand()%10==0) long_walk=2;

                switch(direction)
                {
                    case 0: pos_y_new-=1*long_walk; break;
                    case 1: pos_x_new-=1*long_walk; break;
                    case 2: pos_y_new+=1*long_walk; break;
                    case 3: pos_x_new+=1*long_walk; break;
                }

                //check if outside map
                if(pos_x_new<0 || pos_x_new>=screen_width ||
                   pos_y_new<0 || pos_y_new>=screen_height) continue;

                //check if collision with other unit
                if(grid[pos_x_new][pos_y_new].oos==oos_unit) continue;

                //check if collision with wall
                if(grid[pos_x_new][pos_y_new].oos==oos_wall)
                {
                    //damage wall
                    grid[pos_x_new][pos_y_new].wall_hp-=1;
                    if(grid[pos_x_new][pos_y_new].wall_hp<=0)
                    {
                        //remove
                        grid[pos_x_new][pos_y_new].oos=oos_empty;
                    }

                    continue;
                }

                //check if collision with wall
                if(grid[pos_x_new][pos_y_new].oos==oos_tower)
                {
                    //remove tower
                    grid[pos_x_new][pos_y_new].oos=oos_empty;

                    continue;
                }

                //move unit
                grid[pos_x_new][pos_y_new].oos=oos_unit;
                grid[x][y].oos=oos_empty;
                grid[x][y].already_updated=true;
            }
        }

        //clear updated status
        for(int x=0;x<screen_width;x++)
        for(int y=0;y<screen_height;y++)
        {
            grid[x][y].already_updated=false;
        }
    }
    else unit_move_cooldown-=_time_dif;

    //update towers
    if(tower_fire_cooldown<=0)//fire all towers
    {
        bool play_sound=false;
        tower_fire_cooldown+=_tower_fire_delay;
        for(int x=0;x<screen_width;x++)
        for(int y=0;y<screen_height;y++)
        {
            if(grid[x][y].oos==oos_tower)
            {
                bool has_fired=false;
                //search for enemies nearby
                for(int x_sub=x-_tower_fire_range;x_sub<x+_tower_fire_range;x_sub++)
                for(int y_sub=y-_tower_fire_range;y_sub<y+_tower_fire_range;y_sub++)
                {
                    if(x_sub<0 || y_sub<0 || x_sub>=screen_width || y_sub>=screen_height) continue;

                    if(has_fired) break;

                    if(grid[x_sub][y_sub].oos==oos_unit)
                    {
                        //fire ray
                        vec_rays.push_back(ray(x,y,x_sub,y_sub));

                        //remove unit
                        grid[x_sub][y_sub].oos=oos_empty;

                        //fire only once
                        has_fired=true;

                        play_sound=true;
                    }
                }
            }
        }

        if(play_sound)
            pSound->playSimpleSound(wav_tower_fire,0.2);
    }
    else tower_fire_cooldown-=_time_dif;

    //ship land test
    if(Homebase.landtest(Ship.pos_x,Ship.pos_y,Ship.rotation))
    {
        if(!Ship.landing)
            pSound->playSimpleSound(wav_ship_land,1);

        Ship.landing=true;
    }
    //ship takeoff test
    if(Ship.landing && Ship.speed==0)
    {
        //remove supply
        for(int i=0;i<(int)vec_supplies.size();i++)
        {
            if(vec_supplies[i].in_air)
            {
                vec_supplies.erase(vec_supplies.begin()+i);
                i--;

                supply_meter+=_cost_supply;

                pSound->playSimpleSound(wav_supply_delivery,0.2);
            }
        }

        //any key to takeoff
        for(int i=0;i<256;i++)
        if(keys[i])
        {
            if(Ship.landing)
                pSound->playSimpleSound(wav_ship_takeoff,1);

            Ship.landing=false;
        }
    }

    //ship crash test
    if(Ship.pos_x<0 || Ship.pos_x>=screen_width ||
       Ship.pos_y<0 || Ship.pos_y>=screen_height)
    {
        //ship crash with edge
        game_state=gs_gameover;
        pSound->stopSound(10);
        pSound->playSimpleSound(wav_gameover,1);
    }

    //update level calculation based on time passed
    int minutes_passed=int( game_timer/60.0 );
    level_counter=minutes_passed;

    return true;
}

bool game::draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if(game_state==gs_main)
    {
        glPushMatrix();
        glTranslatef(0,15,0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,tex_main);
        glColor3f(1,1,1);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0,1.0);
        glVertex2f(0,0);
        glTexCoord2f(0.0,0.0);
        glVertex2f(0,screen_height);
        glTexCoord2f(1.0,0.0);
        glVertex2f(screen_width,screen_height);
        glTexCoord2f(1.0,1.0);
        glVertex2f(screen_width,0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        return true;
    }

    if(game_state==gs_info)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,tex_info);
        glColor3f(1,1,1);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0,1.0);
        glVertex2f(0,0);
        glTexCoord2f(0.0,0.0);
        glVertex2f(0,screen_height);
        glTexCoord2f(1.0,0.0);
        glVertex2f(screen_width,screen_height);
        glTexCoord2f(1.0,1.0);
        glVertex2f(screen_width,0);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        return true;
    }

    if(game_state==gs_gameover)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,tex_gameover);
        glColor3f(1,1,1);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0,1.0);
        glVertex2f(0,0);
        glTexCoord2f(0.0,0.0);
        glVertex2f(0,screen_height);
        glTexCoord2f(1.0,0.0);
        glVertex2f(screen_width,screen_height);
        glTexCoord2f(1.0,1.0);
        glVertex2f(screen_width,0);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        //draw score HUD
        int minutes_passed=int( game_timer/60.0 );
        stringstream ss;
        ss<<minutes_passed;
        string minutes_pased_string(ss.str());

        glPushMatrix();
        glTranslatef(screen_width-22,48,0);
        glLineWidth(2);
        glColor3f(0.8,0.8,0.8);
        glScalef(20,20,20);
        for(int i=(int)minutes_pased_string.size();i>=0;i--)
        {
            draw_number(minutes_pased_string[i]);
            glTranslatef(-1.3,0,0);
        }
        glLineWidth(1);
        glPopMatrix();
        //bar
        float seconds_bar=(game_timer-(float)minutes_passed*60.0)/60.0;
        glColor3f(0.7,0.7,0.7);
        glBegin(GL_QUADS);
        glVertex2f(0,30);
        glColor3f(0.5,0.5,0.5);
        glVertex2f(0,35);
        glVertex2f(screen_width*seconds_bar,35);
        glColor3f(0.7,0.7,0.7);
        glVertex2f(screen_width*seconds_bar,30);
        glEnd();

        return true;
    }

    glPushMatrix();
    glColor3f(1,1,1);
    glBegin(GL_POINTS);
    for(int x=0;x<screen_width;x++)
    for(int y=0;y<screen_height;y++)
    {
        switch(grid[x][y].oos)
        {
            case oos_empty:
            {
                glColor3f(grid[x][y].r,grid[x][y].g,grid[x][y].b);
                glVertex2f(x,y);
            }break;

            case oos_unit:
            {
                glColor3f(1,1,1);
                glVertex2f(x,y);
            }break;

            case oos_wall:
            {
                glColor3f(0.5,0.5,0.5);
                glVertex2f(x,y);
            }break;

            case oos_tower:
            {
                glColor3f(0.9,0.9,0.01);
                glVertex2f(x,y);
            }break;
        }
    }
    glEnd();
    glPopMatrix();

    //draw homebase
    Homebase.draw();

    //draw supply
    for(int i=0;i<(int)vec_supplies.size();i++)
    {
        vec_supplies[i].draw();
    }

    //draw bigpixels
    for(int i=0;i<(int)vec_bigpixels.size();i++)
    {
        vec_bigpixels[i].draw();
    }
    glPointSize(1);

    //draw projectiles
    for(int i=0;i<(int)vec_projectiles.size();i++)
    {
        vec_projectiles[i].draw();
    }

    //draw rays
    if(!vec_rays.empty())
    {
        glColor3f(1,1,0.5);
        glBegin(GL_LINES);
        for(int i=0;i<(int)vec_rays.size();i++)
        {
            glVertex2f(vec_rays[i].x_1,vec_rays[i].y_1);
            glVertex2f(vec_rays[i].x_2,vec_rays[i].y_2);
        }
        glEnd();
    }
    vec_rays.clear();

    //draw aim beam
    if(keys[16])
    {
        glColor3f(1,1,1);
        glBegin(GL_LINES);
        glVertex2f(Ship.pos_x,Ship.pos_y);
        glVertex2f(Ship.pos_x+cosf(Ship.rotation*_deg2rad)*2000.0,Ship.pos_y+sinf(Ship.rotation*_deg2rad)*2000.0);
        glEnd();
    }
    //draw ship
    Ship.draw();

    //draw HUD bar 1, pre-resources
    float scale=0.05;
    glColor3f(0.9,0.5,0.2);
    glBegin(GL_QUADS);
    glVertex2f(screen_width*0.5-supply_meter*scale,screen_height-20);
    glColor3f(0.7,0.3,0.0);
    glVertex2f(screen_width*0.5-supply_meter*scale,screen_height-10);
    glVertex2f(screen_width*0.5+supply_meter*scale,screen_height-10);
    glColor3f(0.9,0.5,0.2);
    glVertex2f(screen_width*0.5+supply_meter*scale,screen_height-20);
    glEnd();
    //draw HUD bar 2, refined-resources
    glColor3f(0.8,0.8,0.1);
    glBegin(GL_QUADS);
    glVertex2f(screen_width*0.5-resource_meter*scale,screen_height-10);
    glColor3f(0.5,0.5,0.0);
    glVertex2f(screen_width*0.5-resource_meter*scale,screen_height);
    glVertex2f(screen_width*0.5+resource_meter*scale,screen_height);
    glColor3f(0.8,0.8,0.1);
    glVertex2f(screen_width*0.5+resource_meter*scale,screen_height-10);
    glEnd();

    //draw score HUD
    stringstream ss;
    ss<<level_counter;
    string minutes_pased_string(ss.str());

    glPushMatrix();
    glTranslatef(screen_width-22,48,0);
    glLineWidth(2);
    glColor3f(0.8,0.8,0.8);
    glScalef(20,20,20);
    for(int i=(int)minutes_pased_string.size();i>=0;i--)
    {
        draw_number(minutes_pased_string[i]);
        glTranslatef(-1.3,0,0);
    }
    glLineWidth(1);
    glPopMatrix();
    //bar
    float seconds_bar=(game_timer-(float)level_counter*60.0)/60.0;
    glColor3f(0.7,0.7,0.7);
    glBegin(GL_QUADS);
    glVertex2f(0,30);
    glColor3f(0.5,0.5,0.5);
    glVertex2f(0,35);
    glVertex2f(screen_width*seconds_bar,35);
    glColor3f(0.7,0.7,0.7);
    glVertex2f(screen_width*seconds_bar,30);
    glEnd();

    return true;
}

bool game::reset()
{
    Ship.init();
    Ship.pos_x=screen_width*0.2;
    Ship.pos_y=screen_height*0.5;
    supply_meter=0;
    resource_meter=0;
    game_timer=0;
    level_counter=level_difficulty_tag=0;
    unit_spawner_cooldown=0;
    supply_spawn_cooldown=0;
    vec_projectiles.clear();
    vec_rays.clear();
    vec_spawners.clear();
    vec_supplies.clear();
    vec_bigpixels.clear();
    for(int x=0;x<screen_width;x++)
    for(int y=0;y<screen_height;y++)
    {
        grid[x][y].oos=oos_empty;
    }
    tower_fire_cooldown=1.9;
    pSound->playSimpleSound(wav_song1,1,10,true);

    return true;
}

bool game::draw_number(char val)
{
    switch(val)
    {
        case '0':
        {
            glBegin(GL_LINES);
            glVertex2f(0,0);
            glVertex2f(0,0.5);

            glVertex2f(0,0.5);
            glVertex2f(0,1);

            glVertex2f(0,1);
            glVertex2f(1,1);

            glVertex2f(1,1);
            glVertex2f(1,0.5);

            glVertex2f(1,0.5);
            glVertex2f(1,0);

            glVertex2f(1,0);
            glVertex2f(0,0);
            glEnd();
        }break;

        case '1':
        {
            glBegin(GL_LINES);
            glVertex2f(1,1);
            glVertex2f(1,0.5);

            glVertex2f(1,0.5);
            glVertex2f(1,0);
            glEnd();
        }break;

        case '2':
        {
            glBegin(GL_LINES);
            glVertex2f(0,1);
            glVertex2f(1,1);

            glVertex2f(1,0);
            glVertex2f(0,1);

            glVertex2f(1,0);
            glVertex2f(0,0);
            glEnd();
        }break;

        case '3':
        {
            glBegin(GL_LINES);
            glVertex2f(0,0.5);
            glVertex2f(1,0.5);

            glVertex2f(0,1);
            glVertex2f(1,1);

            glVertex2f(1,1);
            glVertex2f(1,0.5);

            glVertex2f(1,0.5);
            glVertex2f(1,0);

            glVertex2f(1,0);
            glVertex2f(0,0);
            glEnd();
        }break;

        case '4':
        {
            glBegin(GL_LINES);
            glVertex2f(0,0);
            glVertex2f(0,0.5);

            glVertex2f(0,0.5);
            glVertex2f(1,0.5);

            glVertex2f(1,1);
            glVertex2f(1,0.5);

            glVertex2f(1,0.5);
            glVertex2f(1,0);
            glEnd();
        }break;

        case '5':
        {
            glBegin(GL_LINES);
            glVertex2f(0,1);
            glVertex2f(1,1);

            glVertex2f(0,0);
            glVertex2f(1,1);

            glVertex2f(1,0);
            glVertex2f(0,0);
            glEnd();
        }break;

        case '6':
        {
            glBegin(GL_LINES);
            glVertex2f(0,0);
            glVertex2f(0,0.5);

            glVertex2f(0,0.5);
            glVertex2f(0,1);

            glVertex2f(0,1);
            glVertex2f(1,1);

            glVertex2f(1,1);
            glVertex2f(1,0.5);

            glVertex2f(0,0.5);
            glVertex2f(1,0.5);

            glVertex2f(1,0);
            glVertex2f(0,0);
            glEnd();
        }break;

        case '7':
        {
            glBegin(GL_LINES);
            glVertex2f(1,1);
            glVertex2f(1,0.5);

            glVertex2f(1,0.5);
            glVertex2f(1,0);

            glVertex2f(1,0);
            glVertex2f(0,0);
            glEnd();
        }break;

        case '8':
        {
            glBegin(GL_LINES);
            glVertex2f(0,0);
            glVertex2f(0,0.5);

            glVertex2f(0,0.5);
            glVertex2f(0,1);

            glVertex2f(0,1);
            glVertex2f(1,1);

            glVertex2f(1,1);
            glVertex2f(1,0.5);

            glVertex2f(1,0.5);
            glVertex2f(1,0);

            glVertex2f(1,0);
            glVertex2f(0,0);

            glVertex2f(0,0.5);
            glVertex2f(1,0.5);
            glEnd();
        }break;

        case '9':
        {
            glBegin(GL_LINES);
            glVertex2f(0,0);
            glVertex2f(0,0.5);

            glVertex2f(1,1);
            glVertex2f(1,0.5);

            glVertex2f(1,0.5);
            glVertex2f(1,0);

            glVertex2f(1,0);
            glVertex2f(0,0);

            glVertex2f(0,0.5);
            glVertex2f(1,0.5);
            glEnd();
        }break;
    }

    return true;
}
