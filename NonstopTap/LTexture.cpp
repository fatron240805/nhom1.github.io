#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#include "LTexture.h"
#include "RenderWindow.h"

LTexture::LTexture() // initialize
{
    texture = NULL;
    width = 0;
    height = 0;
}

LTexture::~LTexture() // free the memory
{
    free();
}

void LTexture::free() // free the memory
{
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
        width = 0;
        height = 0;
    }
}

void LTexture::updateDimension() // update the size of image
{
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
}

void LTexture::load(std::string path, renderWindow &window) // load an image from link: path and assigns to texture
{
    texture = window.loadTexture(path);
    updateDimension();
}