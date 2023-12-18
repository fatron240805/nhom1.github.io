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

enum GAMEMODE_CODE
{
	ENDURANCE,
	FRENZY,
	PATTERN
};

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

//Check if the game is still being played
extern bool runningGame;

extern bool isPlayingClassic;

extern renderWindow window;

extern mixerManager mixer;

extern TTF_Font* gFont28;
extern TTF_Font* gFont35;
extern TTF_Font* gFontMedium;
extern TTF_Font* gFont70;
extern TTF_Font* gFontBig;
extern TTF_Font* gFontBigger;

extern LTexture logoName;
extern LTexture net;
extern LTexture button;
extern LTexture back;
extern LTexture frame;
extern LTexture rect_frame;
extern LTexture big_frame;
extern LTexture big_round;
extern LTexture big_round_black;
extern LTexture small_round;
extern LTexture small_round_black;
extern LTexture finish;
extern LTexture groupName;
extern LTexture point[6];
extern LTexture result;
extern LTexture replay;
extern LTexture home;

extern SDL_Texture *avatarGroup;

extern std::vector<int>highscore[3];

//Check if the mouse point is inside the hitbox
bool insideHitbox(SDL_Rect rect);

//Loading all images, fonts and sounds
void loadMedia();

//Free resource and quit SDL2
void close();