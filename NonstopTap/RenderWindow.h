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
        renderWindow(); // Initialize 

        renderWindow(std::string titleWindow, int SCREEN_WIDTH, int SCREEN_HEIGHT); // create a window with Title, Width and Height determined by the parameters

        ~renderWindow(); // release the resources

        void free(); // release the resources

        SDL_Texture* loadTexture(std::string filePath); // load a picture from link: filePath to pointer SDL_Texture*

        void cleanScreen(); // clean the screen

        void renderTitle(Title* cell, int r, int g, int b, int a); // render the Title 

        void renderFillRect(int x, int y, int w, int h, int r, int g, int b, int a);

        void render(int x, int y, SDL_Texture* srcTexture, SDL_Rect *clip = NULL, double scale = 1);

        void render(int x, int y, TTF_Font* font, std::string text, SDL_Color textColor, int flag = 0, int l = -1, int r = -1);

        void renderGrid(Grid &currentGrid);

        void display();
    private:
        SDL_Window *gWindow;
        SDL_Renderer *gRender;
};