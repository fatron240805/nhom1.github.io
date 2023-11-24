#include <SDL2/SDL.h>

#include "Title.h"

Title::Title()
{
    colorCell = WHITE_CELL;
    positionStart.x = positionStart.y = 0;
}

Title::Title(int color, int x, int y) {
    colorCell = color;
    positionStart = {x, y};
}

void Title::changeTitle(int color, int x, int y)
{
    colorCell = color;
    positionStart.x = x;
    positionStart.y = y;
}

bool Title::insideCell(int x, int y)
{
    bool inside_x = (positionStart.x <= x && x <= positionStart.x + SIZE_CELL);
    bool inside_y = (positionStart.y <= y && y <= positionStart.y + SIZE_CELL);
    return inside_x && inside_y;
}

bool Title::valid()
{
    return colorCell;
}

bool Title::getColor()
{
    return colorCell;
}

SDL_Point Title::getPosition()
{
    return positionStart;
}

void Title::flipColor()
{
    colorCell ^= 1;
}