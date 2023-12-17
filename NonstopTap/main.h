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

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

enum GAMEMODE_CODE
{
	ENDURANCE,
	FRENZY,
	PATTERN
};

//Check if the mouse point is inside the hitbox
bool insideHitbox(SDL_Rect rect)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return rect.x <= x && x <= rect.x + rect.w && rect.y <= y && y <= rect.y + rect.h;
}

//Initialize SDL2
bool init();

//Loading all images, fonts and sounds
void loadMedia();

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

//Choose gamemode based on some parameters
bool chooseMode(int gamemode, int sizeGrid = 4, Uint64 timeLimit = 30 * 1000, int numBlack = 0);

//Free resource and quit SDL2
void close();