#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#include "RenderWindow.h"

struct LTexture
{
    SDL_Texture *texture;
    int width;
    int height;

    LTexture();

    ~LTexture();

    void free();

    void updateDimension();

    void load(std::string path, renderWindow &window);
};
