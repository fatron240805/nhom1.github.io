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
#include "Var.h"
#include "Gamemode.h"
#include "Screen.h"

//Initialize SDL2
bool init() {
	srand(time(NULL)); 
	
    bool success = true;

	//Initialize SDL2
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "HEY.. SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
        success = false;
    }

	int imgFlag = IMG_INIT_JPG | IMG_INIT_PNG;
	//Initialize SDL_image 
	if (( IMG_Init(imgFlag) & imgFlag) != imgFlag) {
		std::cout << "IMG_init has failed. Error: " << IMG_GetError() << std::endl;
        success = false;
    }

	//Initialize SDL_ttf
	if (TTF_Init() < 0) {
		std::cout << "TTF_init has failed. Error: " << TTF_GetError() << std::endl;
        success = false;
    }

	//Initialize SDL_mixer
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	return success;
}

int main( int argc, char* args[] ){  
    init();

	loadMedia();

	startGame();

	close();

	return EXIT_SUCCESS;
}