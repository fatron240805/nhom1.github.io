#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Title.h"
#include "Grid.h"
#include "RenderWindow.h"


renderWindow::renderWindow()
{
    gWindow = NULL;
    gRender = NULL;
}

renderWindow::renderWindow(std::string titleWindow, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    gWindow = SDL_CreateWindow(titleWindow.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
	gRender = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

renderWindow::~renderWindow()
{
    free();
}

void renderWindow::free()
{
    SDL_DestroyRenderer(gRender);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
    gRender = NULL;
}

SDL_Texture* renderWindow::loadTexture(std::string filePath)
{
    SDL_Texture* newTexture = NULL;
    newTexture = IMG_LoadTexture(gRender, filePath.c_str());
    if (newTexture == NULL)
    {
        std::cerr << "Failed to load texture. Error: " << IMG_GetError() << std::endl;
    }
    return newTexture;
}

void renderWindow::cleanScreen()
{
    SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
    SDL_RenderClear(gRender);
}

void renderWindow::render(int x, int y, SDL_Texture* srcTexture, SDL_Rect *clip, double scale)
{
    SDL_Rect srcClip, dstClip;
    if (clip == NULL)
    {
        srcClip.x = srcClip.y = 0;
        SDL_QueryTexture(srcTexture, NULL, NULL, &srcClip.w, &srcClip.h);
    }
    else
    {
        srcClip = {clip->x, clip->y, clip->w, clip->h};
    }
    //std::cerr << "print size " << srcClip.w << ' ' << srcClip.h << std::endl;

    dstClip = {x, y, int(srcClip.w * scale), int(srcClip.h * scale)};

    SDL_RenderCopy(gRender, srcTexture, &srcClip, &dstClip);
}

void renderWindow::renderTitle(Title* cell)
{
    SDL_Point tmp = cell->getPosition();
    int color = 255 * (1 - cell->getColor());
    renderFillRect(tmp.x, tmp.y, SIZE_CELL, SIZE_CELL, color, color, color, 0);
}

void renderWindow::renderFillRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
    SDL_Rect screenClip = {x, y, w, h};
    SDL_SetRenderDrawColor(gRender, r, g, b, a);
    SDL_RenderFillRect(gRender, &screenClip);
}

void renderWindow::render(int x, int y, TTF_Font* font, std::string text, SDL_Color textColor)
{
    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), textColor, 0);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRender, textSurface);

    //std::cerr << "print word " << text << std::endl;
    render(x, y, textTexture);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    textSurface = NULL;
    textTexture = NULL;
}

void renderWindow::renderGrid(Grid &currentGrid)
{
    SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
    SDL_RenderClear(gRender);

    SDL_SetRenderDrawColor(gRender, 0, 0, 0, 255);
    SDL_RenderFillRect(gRender, currentGrid.getGridRect());

    int sizeGrid = currentGrid.getSize();

    for (int row = 0; row < sizeGrid; row++)
    {
        for (int col = 0; col < sizeGrid; col++)
        {
            SDL_Point tmp = currentGrid.getCoor(row, col);
            SDL_Rect cell = {tmp.x, tmp.y, SIZE_CELL, SIZE_CELL};

            if (currentGrid.getColor(row, col) == WHITE_CELL)
                SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
            else
                SDL_SetRenderDrawColor(gRender, 0, 0, 0, 255);
            SDL_RenderFillRect(gRender, &cell);
        }
    }
}

void renderWindow::display()
{
    SDL_RenderPresent(gRender);
}