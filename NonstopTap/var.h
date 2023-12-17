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

void loadMedia()
{
	//Load texture
	logoName.load("res/gfx/logoName.png", window);
	net.load("res/gfx/net.png", window);
	button.load("res/gfx/button.png", window);
	back.load("res/gfx/back.png", window);
	frame.load("res/gfx/bigframe3.png", window);
	rect_frame.load("res/gfx/square_button.png", window);
	big_frame.load("res/gfx/bigframe5.png", window);
	big_round.load("res/gfx/big_round1.png", window);
	big_round_black.load("res/gfx/big_round2.png", window);
	small_round.load("res/gfx/small_round1.png", window);
	small_round_black.load("res/gfx/small_round2.png", window);
	result.load("res/gfx/result.png", window);
	finish.load("res/gfx/finish_frame.png", window);
	replay.load("res/gfx/replay.png", window);
	home.load("res/gfx/home.png", window);
	groupName.load("res/gfx/groupName.png", window);

	//load 5 image +1, +2, ..., +5 to point[1], point[2], ..., point[5]
	for (int i = 1; i <= 5; i++)
	{
		std::string path = "res/gfx/+";
		path += char(i + 48);
		path += ".png";
		//std::cerr << path << std::endl;
		point[i].load(path, window);
	}

	avatarGroup = window.loadTexture("res/gfx/avatarGroup.png");

	//Load fonts
	gFont28 = TTF_OpenFont("res/font/bungee.ttf", 28);
	gFont35 = TTF_OpenFont("res/font/bungee.ttf", 35);
	gFontMedium = TTF_OpenFont("res/font/bungee.ttf", 50);
	gFont70 = TTF_OpenFont("res/font/bungee.ttf", 70);
	gFontBig = TTF_OpenFont("res/font/bungee.ttf", 100);
	gFontBigger = TTF_OpenFont("res/font/bungee.ttf", 200);

	//Load sounds
	mixer.loadMenuGameSound("res/sfx/menu.mp3");
	mixer.addRightNoteSound("res/sfx/click0.wav");
	mixer.addRightNoteSound("res/sfx/click1.wav");
	mixer.addRightNoteSound("res/sfx/click2.wav");
	mixer.loadWrongNoteound("res/sfx/failed.wav");
}

void close()
{
	//Free window and renderer
	window.free();

	//Free texture
	logoName.free();
	net.free();
	button.free();
	back.free();
	frame.free();
	rect_frame.free();
	big_frame.free();
	big_round.free();
	big_round_black.free();
	small_round.free();
	small_round_black.free();
	result.free();
	finish.free();
	replay.free();
	home.free();
	groupName.free();
	for (int i = 1; i <= 5; i++)
	{
		point[i].free();
	}
	SDL_DestroyTexture(avatarGroup);

	//Free font
	TTF_CloseFont(gFont28);
	gFont28 = NULL;
	TTF_CloseFont(gFont35);
	gFont35 = NULL;
	TTF_CloseFont(gFontMedium);
	gFontMedium = NULL;
	TTF_CloseFont(gFont70);
	gFont70 = NULL;
	TTF_CloseFont(gFontBig);
	gFontBig = NULL;
	TTF_CloseFont(gFontBigger);
	gFontBigger = NULL;

	//Free sound
	mixer.free();

	//Free the rest
	IMG_Quit();
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
}