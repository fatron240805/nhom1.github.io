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

//Check if the game is still being played
bool runningGame = true;

bool isPlayingClassic = true;

//Used to render everything in screen
renderWindow window("Nonstop Tap", SCREEN_WIDTH, SCREEN_HEIGHT);

//Manager sound effect and BGM
mixerManager mixer = mixerManager();

//Font
TTF_Font* gFont28 = NULL;
TTF_Font* gFont35 = NULL;
TTF_Font* gFontMedium = NULL;
TTF_Font* gFont70 = NULL;
TTF_Font* gFontBig = NULL;
TTF_Font* gFontBigger = NULL;

//Image
LTexture logoName = LTexture();
LTexture net = LTexture();
LTexture button = LTexture();
LTexture back = LTexture();
LTexture frame = LTexture();
LTexture rect_frame = LTexture();
LTexture big_frame = LTexture();
LTexture big_round = LTexture();
LTexture big_round_black = LTexture();
LTexture small_round = LTexture();
LTexture small_round_black = LTexture();
LTexture finish = LTexture();
LTexture groupName = LTexture();
LTexture point[6]{};
LTexture result = LTexture();
LTexture replay = LTexture();
LTexture home = LTexture();

SDL_Texture *avatarGroup = NULL;

//Highscore from playing classic gamemode
std::vector<int>highscore[3];