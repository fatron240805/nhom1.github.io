#pragma once
#include <SDL2/SDL.h>

const int WHITE_CELL = 0;
const int BLACK_CELL = 1;
const int SIZE_CELL = 90;

class Title // use to manage cells
{
    public:
        Title(); // initialize without parameters

        Title(int color, int x, int y); // setup the title's properties (initialize with parameters)

        void changeTitle(int color, int x, int y); // change the title's properties

        bool insideCell(int x, int y); // Check if the point (x, y) is inside cell with position (positionStart.x, positionStart.y)

        bool valid(); // check if title is valid or not

        bool getColor(); // get the color of cell

        SDL_Point getPosition(); // get the positionStart of cell

        void flipColor(); // reverse the color (white -> black and black -> white)

    private:
        bool colorCell; // color of cell (0 if while and 1 if black)
        SDL_Point positionStart; // the point is in the upper left corner of the cell
};