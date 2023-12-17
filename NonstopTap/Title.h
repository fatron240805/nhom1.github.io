#pragma once
#include <SDL2/SDL.h>

const int WHITE_CELL = 0;
const int BLACK_CELL = 1;
const int SIZE_CELL = 90;

// use to manage cells
class Title 
{
    public:
        // initialize without parameters
        Title(); 

        // setup the title's properties (initialize with parameters)
        Title(int color, int x, int y); 

        // change the title's properties
        void changeTitle(int color, int x, int y); 

        // Check if the point (x, y) is inside cell with position (positionStart.x, positionStart.y)
        bool insideCell(int x, int y); 

        // check if title is valid or not
        bool valid(); 

        // get the color of cell
        bool getColor(); 

        // get the positionStart of cell
        SDL_Point getPosition(); 

        // reverse the color (white -> black and black -> white)
        void flipColor(); 

    private:
        bool colorCell; // color of cell (0 if while and 1 if black)
        SDL_Point positionStart; // the point is in the upper left corner of the cell
};