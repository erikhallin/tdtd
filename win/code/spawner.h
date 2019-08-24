#ifndef SPAWNER_H
#define SPAWNER_H

class spawner
{
    public:
        spawner(int pos_x,int pos_y);

        bool update();
        bool draw();

        int pos_x,pos_y;
        float spawn_cooldown;
        float spawn_delay;
        float lifetime;

    private:
};

#endif // SPAWNER_H
