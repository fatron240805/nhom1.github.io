#pragma once
#include <SDL2/SDL.h>

const int WHITE_CELL = 0;
const int BLACK_CELL = 1;
const int SIZE_CELL = 90;

class Title
{
    public:
        Title();

        Title(int color, int x, int y);

        void changeTitle(int color, int x, int y);

        bool insideCell(int x, int y);

        bool valid();

        bool getColor();

        SDL_Point getPosition();

        void flipColor();

    private:
        bool colorCell;
        SDL_Point positionStart;
};