#pragma once
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Title.h"
#include "Grid.h"

class renderWindow
{
    public:
        renderWindow();

        renderWindow(std::string titleWindow, int SCREEN_WIDTH, int SCREEN_HEIGHT);

        ~renderWindow();

        void free();

        SDL_Texture* loadTexture(std::string filePath);

        void cleanScreen();

        void renderTitle(Title* cell, int r, int g, int b, int a);

        void renderFillRect(int x, int y, int w, int h, int r, int g, int b, int a);

        void render(int x, int y, SDL_Texture* srcTexture, SDL_Rect *clip = NULL, double scale = 1);

        void render(int x, int y, TTF_Font* font, std::string text, SDL_Color textColor, int flag = 0, int l = -1, int r = -1);

        void renderGrid(Grid &currentGrid);

        void display();
    private:
        SDL_Window *gWindow;
        SDL_Renderer *gRender;
};