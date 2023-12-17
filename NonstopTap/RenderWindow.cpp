#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Title.h"
#include "Grid.h"
#include "RenderWindow.h"


renderWindow::renderWindow() // Initialize 
{
    gWindow = NULL;
    gRender = NULL;
}

renderWindow::renderWindow(std::string titleWindow, int SCREEN_WIDTH, int SCREEN_HEIGHT) // create a window
{   
    // create a window with Title, Width, Height, a coordinate axis with the positions of x, y are random, and support high resolution
    gWindow = SDL_CreateWindow(titleWindow.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    // create a renderer of gWindow
	gRender = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

renderWindow::~renderWindow() // release the resources
{
    free();
}

void renderWindow::free() // release the resources
{
    SDL_DestroyRenderer(gRender); // destroy the renderer gRender
	SDL_DestroyWindow(gWindow); // destroy the window gWindow
	gWindow = NULL;
    gRender = NULL;
}

SDL_Texture* renderWindow::loadTexture(std::string filePath) // load a picture from link: filePath to pointer SDL_Texture*

{
    SDL_Texture* newTexture = NULL;
    newTexture = IMG_LoadTexture(gRender, filePath.c_str());
    if (newTexture == NULL)
    {
        std::cerr << "Failed to load texture. Error: " << IMG_GetError() << std::endl;
    }
    return newTexture;
}

void renderWindow::cleanScreen() // clean the screen
{
    SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255); // set white colour for the renderer
    SDL_RenderClear(gRender);
}

void renderWindow::renderTitle(Title* cell, int r, int g, int b, int a)
{
    SDL_Point tmp = cell->getPosition(); // get the coordinates (x, y) of cell
    //std::cerr << "info " << tmp.x << ' ' << tmp.y << ' ' << r << ' ' << g << ' ' << b << ' ' << a << std::endl;
    renderFillRect(tmp.x, tmp.y, SIZE_CELL, SIZE_CELL, r, g, b, a); // render a rectangle to the screen at coordinates x, y
}

void renderWindow::renderFillRect(int x, int y, int w, int h, int r, int g, int b, int a) // render a filled rectangle to the screen at coordinates x, y
{
    SDL_Rect screenClip = {x, y, w, h}; // a rectangle with x, y coordinates and width w, height h
    SDL_SetRenderDrawColor(gRender, r, g, b, a); // set the color of renderer 
    SDL_RenderFillRect(gRender, &screenClip); // render a filled rectangle to the screen, with pre-set color
}

void renderWindow::render(int x, int y, SDL_Texture* srcTexture, SDL_Rect *clip, double scale) // zoom in/out with a ratio of scale and render to the screen at coordinates x, y
{
    SDL_Rect srcClip, dstClip; // declaration two rectangle
    if (clip == NULL) // check if the pointer clip is not point to any SDL_Rect object
    {
        srcClip.x = srcClip.y = 0;
        SDL_QueryTexture(srcTexture, NULL, NULL, &srcClip.w, &srcClip.h); // query the width and height of srcTexture
    }
    else
    {
        srcClip = {clip->x, clip->y, clip->w, clip->h}; // set the srcClip equal to the rectangle that clip points to 
    }
    //std::cerr << "print size " << srcClip.w << ' ' << srcClip.h << std::endl;

    dstClip = {x, y, int(srcClip.w * scale), int(srcClip.h * scale)}; // set the dstClip like srcClip with a ratio of scale

    SDL_RenderCopy(gRender, srcTexture, &srcClip, &dstClip); // copy texture and render onto the rectangular area dstClip
}

void renderWindow::render(int x, int y, TTF_Font* font, std::string text, SDL_Color textColor, int flag, int l, int r) //
{
    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), textColor, 0);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRender, textSurface);

    //std::cerr << "print word " << text << std::endl;
    if (flag == 0)
    {
        render(x, y, textTexture);
    }
    else
    {
        if (flag == 1) // Align left
        {
            render(l, y, textTexture);
        }
        else if (flag == 2) // Align right
        {
            int tmp;
            SDL_QueryTexture(textTexture, NULL, NULL, &tmp, NULL);
            render(r - tmp, y, textTexture);
        }
        else if (flag == 3) // Center
        {
            int tmp;
            SDL_QueryTexture(textTexture, NULL, NULL, &tmp, NULL);
            render(l + (r - l - tmp) / 2, y, textTexture);
        }
    }

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    textSurface = NULL;
    textTexture = NULL;
}

void renderWindow::renderGrid(Grid &currentGrid) // render the grid
{
    SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255); // set white colour for the renderer
    SDL_RenderClear(gRender); // clear all the screen with white colour

    SDL_SetRenderDrawColor(gRender, 0, 0, 0, 255); // set black colour for the renderer
    SDL_RenderFillRect(gRender, currentGrid.getGridRect()); // render a filled rectangle with position and size like currentGrid

    int sizeGrid = currentGrid.getSize(); // get the size of currentGrid

    for (int row = 0; row < sizeGrid; row++)
    {
        for (int col = 0; col < sizeGrid; col++)
        {
            SDL_Point tmp = currentGrid.getCoor(row, col); // store the position of point in the upper left corner of a sub-square of the grid
            SDL_Rect cell = {tmp.x, tmp.y, SIZE_CELL, SIZE_CELL}; // store a square with side length SIZE_CELL

            if (currentGrid.getColor(row, col) == WHITE_CELL) // check if the color of the cell is white
                SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255); // set white colour for the renderer
            else // check when the color of the cell is black
                SDL_SetRenderDrawColor(gRender, 0, 0, 0, 255); // set black colour for the renderer
            
            SDL_RenderFillRect(gRender, &cell); // render a filled square to the screen, at the position of cell, and with pre-set color
        }
    }
}

void renderWindow::display() // display the contents of graphic window on the screen
{
    SDL_RenderPresent(gRender);
}