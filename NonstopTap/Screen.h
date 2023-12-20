#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>
#include <assert.h>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "RenderWindow.h"
#include "MixerManager.h"
#include "LTexture.h"
#include "Var.h"
#include "Gamemode.h"

//Draw background, slightly move down for each frame
void drawBackground();

//Start screen
void startGame();

//About us: image of each teammate
void aboutUs();

//Show gamemode in here
void menu();

//Show 3 classic gamemodes
void classicGamemode();

//Show the "Option" option and "Random" option
void advanceGamemode();

//Allowed player to choose parameters for each property
bool optionMap(int &gamemode, int &sizeGrid, Uint64 &timeLimit);