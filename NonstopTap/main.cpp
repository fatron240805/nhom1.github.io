#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <assert.h>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "Title.h"
#include "Grid.h"
#include "RenderWindow.h"
#include "MixerManager.h"

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

//Check if the game is still be played
bool runningGame = true;

//Initialize SDL2
bool init();

//Loading all image and sound
void loadMedia();

//Start screen
void startGame();

//About us
void aboutUs();

//Show gamemode in here
void menu();

//Choose gamemode based on some parameters
bool chooseMode(int gamemode, int sizeGrid, Uint64 timeLimit, int numBlack);

//
void waitingForStart(int gamemode);

//Endurance mode: keep playing as long as possible
bool enduranceMode(int sizeGrid, Uint64 timeLimit, int numBlack);

//Frenzy mode: get highest score in given time
bool frenzyMode(int sizeGrid, Uint64 timeLimit, int numBlack);

//Pattern mode: complete every pattern in screen
bool patternMode(int sizeGrid, Uint64 timeLimit, int numBlack);

//Get response
bool getResponse();

//Free resource and quit SDL2
void close();

bool init() {
    bool success = true;

	//Initialize SDL2
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "HEY.. SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
        success = false;
    }

	//Initialize SDL_image (PNG)
	if (!( IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
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

//Used to render everything in screen
renderWindow window("Nonstop Tap", SCREEN_WIDTH, SCREEN_HEIGHT);
mixerManager mixer = mixerManager();

//Font
TTF_Font* gFont28 = NULL;

void loadMedia()
{
	gFont28 = TTF_OpenFont("res/font/FallingSkyBlack-GYXA.otf", 28);
	mixer.loadMenuGameSound("res/sfx/menu_sound.mp3");
	mixer.addRightNoteSound("res/sfx/click0.wav");
	mixer.addRightNoteSound("res/sfx/click1.wav");
}

void startGame()
{
	SDL_Event e;

	bool quit = false;

	while (!quit && runningGame)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
			{
				runningGame = false;
			}
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)
			{
				mixer.stopPlayMenuSound();
				aboutUs();
			}
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_a)
			{
				mixer.stopPlayMenuSound();
				menu();
			}
		}

		mixer.playMenuSound();
		window.cleanScreen();
		SDL_Color BLACK = {0, 0, 0, 255};
		window.render(0, 0, gFont28, "This is home. Press Q to know about us or A to start playing game", BLACK);
		window.display();
	}
}

void aboutUs()
{
	SDL_Event e;

	bool quit = false;

	while (!quit && runningGame)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_e)
			{
				quit = true;
			}
		}

		window.cleanScreen();
		SDL_Color BLACK = {0, 0, 0, 255};
		window.render(0, 0, gFont28, "This is home. Press E to back to home", BLACK);
		window.display();
	}
}

void menu()
{
	SDL_Event e;

	bool quit = false;
	
	bool keepPlaying = false;
	int gameMode = 1;
	int sizeGrid = 4;
	Uint64 timeLimit = 30 * 1000;
	int numBlack = 3;

	while (!quit && runningGame)
	{
		while (keepPlaying)
		{
			keepPlaying = chooseMode(gameMode, sizeGrid, timeLimit, numBlack);
		}
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				else
				{
					switch (e.key.keysym.sym)
					{
						case SDLK_0:
							gameMode = 0;
							sizeGrid = 4;
							timeLimit = 10 * 1000;
							numBlack = 3;
							break;
						
						case SDLK_1:
							gameMode = 1;
							sizeGrid = 4;
							timeLimit = 30 * 1000;
							numBlack = 3;
							break;
						
						case SDLK_2:
							gameMode = 2;
							sizeGrid = 7;
							timeLimit = 30 * 1000;
							numBlack = 3;
							break;
						
						case SDLK_3:
							gameMode = (rand() % 3);
							sizeGrid = 4 + (rand() % 4);
							timeLimit = (1 + (rand() % 4)) * 15 * 1000;
							numBlack = 3 + (rand() % 4);
							break;
						
						default:
							break;
					}

					keepPlaying = chooseMode(gameMode, sizeGrid, timeLimit, numBlack);
				}
			}
		}

		if (!keepPlaying)
		{
			window.cleanScreen();
			SDL_Color BLACK = {0, 0, 0, 255};
			window.render(0, 0, gFont28, "ESC: home, or 0: Endurance, 1: Frenzy, 2: Pattern, 3: Random", BLACK);
			window.display();
		}
	}
}

void waitingForStart(int gamemode)
{
	Uint64 timeStart = SDL_GetTicks64();
	SDL_Color BLACK = {0, 0, 0, 255};
	while (true)
	{
		window.cleanScreen();
		Uint64 amountTime = SDL_GetTicks64() - timeStart;
		if (amountTime <= 1000)
		{
			if (gamemode == 0)
			{
				window.render(0, 0, gFont28, "Play as long as possible", BLACK);
			}
			else if (gamemode == 1)
			{
				window.render(0, 0, gFont28, "Get the highest score in limited time", BLACK);
			}
			else 
			{
				assert(gamemode == 2);
				window.render(0, 0, gFont28, "Clear all pattern as fast as possible", BLACK);
			}
		}
		else if (amountTime <= 2000)
		{
			window.render(0, 0, gFont28, "3", BLACK);
		}
		else if (amountTime <= 3000)
		{
			window.render(0, 0, gFont28, "2", BLACK);
		}
		else if (amountTime <= 4000)
		{
			window.render(0, 0, gFont28, "1", BLACK);
		}
		else if (amountTime <= 5000)
		{
			window.render(0, 0, gFont28, "0", BLACK);
		}
		else
		{
			break;
		}
	}
	
}

bool chooseMode(int gamemode, int sizeGrid, Uint64 timeLimit, int numBlack)
{
	if (gamemode == 0)
	{
		return enduranceMode(sizeGrid, timeLimit, numBlack);
	}
	else if (gamemode == 1)
	{
		return frenzyMode(sizeGrid, timeLimit, numBlack);
	}
	else 
	{
		assert(gamemode == 2);
		return patternMode(sizeGrid, timeLimit, numBlack);
	}
}

bool enduranceMode(int sizeGrid, Uint64 timeLimit, int numBlack)
{
	Grid currentGrid = Grid(sizeGrid, timeLimit, numBlack, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Event e;

	bool quit = false;

	const int amountTimeBonus = 10 * 1000;
	int score = 0;
	int requiredScore = numBlack * (1 + amountTimeBonus / 1000);
	
	Uint64 timeStart = SDL_GetTicks64();
	Uint64 timeBonus = 0;

	std::stringstream textStream;
	SDL_Color colorText = {0, 0, 0, 255};

	while (!quit && runningGame)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				std::pair<int, int> coorCell = currentGrid.identifyCell();
				if (coorCell.first != -1 && coorCell.second != -1)
				{
					bool action = currentGrid.validAction(coorCell);
					if (action)
					{
						currentGrid.changeColor(coorCell.first, coorCell.second);
						if (++score % requiredScore == 0)
						{
							timeBonus += amountTimeBonus;
						}
					}
					else
					{
						quit = true;
					}
				}
			}
		}

		if (SDL_GetTicks64() - timeStart >= timeLimit + timeBonus)
		{
			quit = true;
		}

		window.renderGrid(currentGrid);

		textStream.str("");
		textStream << "Current score: " << score;
		window.render(0, 0, gFont28, textStream.str().c_str(), colorText);

		textStream.str("");
		textStream << "Time left: " << 1.0 * (timeLimit + timeBonus - SDL_GetTicks64() + timeStart) / 1000;
		window.render(0, SCREEN_HEIGHT - 28, gFont28, textStream.str().c_str(), colorText);
		window.display();
	}

	window.cleanScreen();
	textStream.str("");
	textStream << "Your score: " << score << ". Press R to play again or T to choose another gamemode";
	window.render(0, 0, gFont28, textStream.str().c_str(), colorText);
	window.display();

	return getResponse();
}

bool frenzyMode(int sizeGrid, Uint64 timeLimit, int numBlack)
{
	Grid currentGrid = Grid(sizeGrid, timeLimit, numBlack, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Event e;

	bool quit = false;

	int score = 0;

	Uint64 timeStart = SDL_GetTicks64();

	std::stringstream textStream;
	SDL_Color colorText = {0, 0, 0, 255};

	while (!quit && runningGame)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				std::pair<int, int> coorCell = currentGrid.identifyCell();
				if (coorCell.first != -1 && coorCell.second != -1)
				{
					bool action = currentGrid.validAction(coorCell);
					if (action)
					{
						currentGrid.changeColor(coorCell.first, coorCell.second);
						score++;
					}
					else
					{
						quit = true;
					}
				}
			}
		}

		if (SDL_GetTicks64() - timeStart >= timeLimit)
		{
			quit = true;
		}

		window.renderGrid(currentGrid);

		textStream.str("");
		textStream << "Current score: " << score;
		window.render(0, 0, gFont28, textStream.str().c_str(), colorText);

		textStream.str("");
		textStream << "Time left: " << 1.0 * (timeLimit - SDL_GetTicks64() + timeStart) / 1000;
		window.render(0, SCREEN_HEIGHT - 28, gFont28, textStream.str().c_str(), colorText);
		window.display();
	}

	window.cleanScreen();
	textStream.str("");
	textStream << "Your score: " << score << ". Press R to play again or T to choose another gamemode";
	window.render(0, 0, gFont28, textStream.str().c_str(), colorText);
	window.display();

	return getResponse();
}

bool patternMode(int sizeGrid, Uint64 timeLimit, int numBlack)
{
	Grid currentGrid = Grid(sizeGrid, timeLimit, numBlack, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Event e;

	bool quit = false;

	int numPattern = 10;
	int leftBlack = numBlack;

	Uint64 timeStart = SDL_GetTicks64();

	std::stringstream textStream;
	SDL_Color colorText = {0, 0, 0, 255};

	window.renderGrid(currentGrid);

	while (!quit && runningGame)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				std::pair<int, int> coorCell = currentGrid.identifyCell();
				if (coorCell.first != -1 && coorCell.second != -1)
				{
					bool action = currentGrid.validAction(coorCell);
					if (action)
					{
						mixer.playRightNoteSound();
						currentGrid.onlyChangeColor(coorCell.first, coorCell.second);
						leftBlack--;
					}
					else
					{
						quit = true;
					}
				}
			}
		}

		if (numPattern == 1 && leftBlack == 0)
		{
			quit = true;
		}
		else 
		{
			std::cerr << numPattern << ' ' << leftBlack << std::endl;
			if (leftBlack == 0)
			{
				currentGrid.generateBlackCell();
				leftBlack = numBlack;
				numPattern--;
			}
			window.renderGrid(currentGrid);
		}
		textStream.str("");
		textStream << "Time: " << 1.0 * (SDL_GetTicks64() - timeStart) / 1000;
		window.render(0, SCREEN_HEIGHT - 28, gFont28, textStream.str().c_str(), colorText);
		window.display();
	}

	window.cleanScreen();
	textStream.str("");
	textStream << "Time: " << 1.0 * (SDL_GetTicks64() - timeStart) / 1000 << ". Press R to play again or T to choose another gamemode";
	window.render(0, 0, gFont28, textStream.str().c_str(), colorText);
	window.display();

	return getResponse();
}

bool getResponse()
{
	SDL_Event e;

	while(runningGame)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
				return false;
			}
			
			else if (e.type == SDL_KEYDOWN)
			{
				if (e.key.keysym.sym == SDLK_r)
				{
					return true;
				}
				else if (e.key.keysym.sym == SDLK_t)
				{
					return false;
				}
			}
		}
	}
	return true;
}

void close()
{
	//Free window and renderer
	window.free();

	//Free the rest
	IMG_Quit();
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] ){
	srand(time(NULL));   

    init();

	loadMedia();

	startGame();

	close();

	return EXIT_SUCCESS;
}
