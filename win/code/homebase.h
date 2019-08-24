#ifndef HOMEBASE_H
#define HOMEBASE_H

#include <gl/gl.h>

class homebase
{
    public:
        homebase();
        homebase(int pos_x,int pos_y);

        int pos_x,pos_y;
        bool click_flag;

        bool landtest(int pos_x,int pos_y,float rotation);

        bool draw();

    private:
};

#endif // HOMEBASE_H
