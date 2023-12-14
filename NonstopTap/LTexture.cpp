#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#include "LTexture.h"
#include "RenderWindow.h"

LTexture::LTexture()
{
    texture = NULL;
    width = 0;
    height = 0;
}

LTexture::~LTexture()
{
    free();
}

void LTexture::free()
{
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
        width = 0;
        height = 0;
    }
}

void LTexture::updateDimension()
{
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
}

void LTexture::load(std::string path, renderWindow &window)
{
    texture = window.loadTexture(path);
    updateDimension();
}