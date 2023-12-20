#pragma once
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Title.h"
#include "Grid.h"

// render text and images on screen
class renderWindow 
{
    public:
        // Initialize 
        renderWindow(); 

        // create a window with Title, Width and Height determined by the parameters
        renderWindow(std::string titleWindow, int SCREEN_WIDTH, int SCREEN_HEIGHT); 

        // release the resources
        ~renderWindow(); 

        // release the resources
        void free(); 

        // load a picture from link: filePath to pointer SDL_Texture*
        SDL_Texture* loadTexture(std::string filePath); 

        // clean the screen
        void cleanScreen(); 

        // render the Title 
        void renderTitle(Title* cell, int r, int g, int b, int a); 

        // render a filled rectangle to the screen at coordinates x, y
        void renderFillRect(int x, int y, int w, int h, int r, int g, int b, int a);

        // zoom in/out with a ratio of scale and render to the screen at coordinates x, y
        void render(int x, int y, SDL_Texture* srcTexture, SDL_Rect *clip = NULL, double scale = 1);

        // display text on screen 
        void render(int x, int y, TTF_Font* font, std::string text, SDL_Color textColor, int flag = 0, int l = -1, int r = -1);

        // render the grid
        void renderGrid(Grid &currentGrid);

        // display the contents of graphic window on the screen
        void display();
    private:
        SDL_Window *gWindow; // a pointer to stores the graphic window information
        SDL_Renderer *gRender; // a pointer to render graphic objects 
};