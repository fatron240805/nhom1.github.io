#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "LTexture.h"

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
        width = 0;
        height = 0;
    }
}

void LTexture::updateDimension()
{
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
}