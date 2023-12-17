#include <SDL2/SDL.h>

#include "Title.h"

Title::Title() // initialize without parameters
{
    colorCell = WHITE_CELL;
    positionStart.x = positionStart.y = 0;
}

Title::Title(int color, int x, int y) // setup the title's properties (initialize with parameters)
{
    colorCell = color;
    positionStart = {x, y};
}

void Title::changeTitle(int color, int x, int y) // change the title's properties
{
    colorCell = color;
    positionStart.x = x;
    positionStart.y = y;
}

bool Title::insideCell(int x, int y) // Check if the point (x, y) is inside cell with position (positionStart.x, positionStart.y)
{
    bool inside_x = (positionStart.x <= x && x <= positionStart.x + SIZE_CELL); 
    bool inside_y = (positionStart.y <= y && y <= positionStart.y + SIZE_CELL);
    return inside_x && inside_y;
}

bool Title::valid() // check if title is valid or not
{
    return colorCell;
}

bool Title::getColor() // get the color of cell
{
    return colorCell;
}

SDL_Point Title::getPosition() // get the positionStart of cell
{
    return positionStart;
}

void Title::flipColor() // reverse the color (white -> black and black -> white)
{
    colorCell ^= 1;
}