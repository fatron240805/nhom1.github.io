#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#include "RenderWindow.h"

// manage the texture of image
struct LTexture
{
    // to store images 
    SDL_Texture *texture; 
    // width of image
    int width;
    // height of image
    int height;

    // initialize
    LTexture(); 

    // free the memory
    ~LTexture();

    // free the memory
    void free();

    // update the size of image
    void updateDimension();

    // load an image from link: path and assigns to texture
    void load(std::string path, renderWindow &window); 
};
