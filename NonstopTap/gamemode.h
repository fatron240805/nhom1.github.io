#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <array>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "Title.h"
#include "Grid.h"
#include "RenderWindow.h"
#include "MixerManager.h"
#include "LTexture.h"
#include "main.h"
#include "var.h"

//Give player some seconds to prepare
bool waitingForStart(int gamemode);

//Endurance mode: keep playing as long as possible
bool enduranceMode(int sizeGrid, Uint64 timeLimit, int numBlack);

//Frenzy mode: get highest score in given time
bool frenzyMode(int sizeGrid, Uint64 timeLimit, int numBlack);

//Pattern mode: complete every pattern in screen
bool patternMode(int sizeGrid, Uint64 timeLimit, int numBlack);

//Get response from player
bool getResponse(int gamemode, int currentScore);