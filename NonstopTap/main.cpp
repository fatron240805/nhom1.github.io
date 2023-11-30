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

//Check if the game is still be played
bool runningGame = true;

bool insideHitbox(SDL_Rect rect)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	return rect.x <= x && x <= rect.x + rect.w && rect.y <= y && y <= rect.y + rect.h;
}

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

void classicGamemode();

void advanceGamemode();

//Choose gamemode based on some parameters
bool chooseMode(int gamemode, int sizeGrid = 4, Uint64 timeLimit = 30 * 1000, int numBlack = 3);

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

	int imgFlag = IMG_INIT_JPG | IMG_INIT_PNG;
	//Initialize SDL_image (PNG)
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

//Used to render everything in screen
renderWindow window("Nonstop Tap", SCREEN_WIDTH, SCREEN_HEIGHT);
mixerManager mixer = mixerManager();

//Font
TTF_Font* gFont28 = NULL;
TTF_Font* gFontBig = NULL;

//Image
LTexture logoName = LTexture();
LTexture net = LTexture();
LTexture button = LTexture();
LTexture back = LTexture();
LTexture frame = LTexture();

LTexture avatarBao = LTexture();

SDL_Cursor* cursor;

void loadMedia()
{
	logoName.texture = window.loadTexture("res/gfx/logoName.png");
	logoName.updateDimension();

	net.texture = window.loadTexture("res/gfx/net.png");
	net.updateDimension();

	button.texture = window.loadTexture("res/gfx/button.png");
	button.updateDimension();

	back.texture = window.loadTexture("res/gfx/back.png");
	back.updateDimension();

	frame.texture = window.loadTexture("res/gfx/bigframe2.png");
	frame.updateDimension();

	gFont28 = TTF_OpenFont("res/font/bungee.ttf", 28);
	gFontBig = TTF_OpenFont("res/font/bungee.ttf", 100);

	mixer.loadMenuGameSound("res/sfx/menu_sound.mp3");
	mixer.addRightNoteSound("res/sfx/click0.wav");
	mixer.addRightNoteSound("res/sfx/click1.wav");
	mixer.loadWrongNoteound("res/sfx/failed01.wav");
}

void drawBackground()
{
	static int frame = 0;

	//std::cerr << frame << std::endl;

	mixer.playMenuSound();
	window.cleanScreen();
	
	assert(net.height >= SCREEN_HEIGHT);
	if (frame <= net.height - SCREEN_HEIGHT)
	{
		SDL_Rect clip = {0, frame, SCREEN_WIDTH, SCREEN_HEIGHT};
		window.render(0, 0, net.texture, &clip);
	}
	else
	{
		SDL_Rect upper = {0, frame, SCREEN_WIDTH, net.height - frame};
		SDL_Rect lower = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - (net.height - frame)};
		window.render(0, 0, net.texture, &upper);
		window.render(0, net.height - frame, net.texture, &lower);
	}

	frame = (frame + 1) % net.height;
}

void startGame()
{
	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(cursor);

	SDL_Event e;

	bool quit = false;
	int tmp = 0;

	while (!quit && runningGame)
	{
		SDL_Rect menuButton = {(SCREEN_WIDTH - button.width) / 2, 450, button.width, button.height};
		SDL_Rect aboutUsButton = {(SCREEN_WIDTH - button.width) / 2, 600, button.width, button.height};

		if (insideHitbox(menuButton) || insideHitbox(aboutUsButton))
		{
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		}
		else
		{
			cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		}
		SDL_SetCursor(cursor);

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
			{
				runningGame = false;
			}
			else if (insideHitbox(menuButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					menu();
				}
			}
			else if (insideHitbox(aboutUsButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					aboutUs();
				}
			}
		}

		drawBackground();

		window.render((SCREEN_WIDTH - logoName.width) / 2, 50, logoName.texture);

		SDL_Color BLACK = {0, 0, 0, 255};

		window.render(menuButton.x, menuButton.y, button.texture);
		std::cerr << menuButton.x << ' ' << menuButton.y << ' ' << menuButton.w << ' ' << menuButton.h << std::endl;
		window.render(menuButton.x + 95, menuButton.y + 25, gFont28, "MENU", BLACK);

		window.render(aboutUsButton.x, aboutUsButton.y, button.texture);
		window.render(menuButton.x + 62, menuButton.y + 175, gFont28, "ABOUT US", BLACK);

		window.display();
	}
}

void aboutUs()
{
	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(cursor);

	std::cerr << "about us" << std::endl;
	mixer.playMenuSound();
	SDL_Event e;

	bool quit = false;

	while (!quit && runningGame)
	{	
		SDL_Rect backButton = {20, 20, back.width, back.height};

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			else if (insideHitbox(backButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					quit = true;
				}
			}
		}

		drawBackground();

		window.render(20, 20, back.texture);

		window.display();
	}
}

void menu()
{
	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(cursor);

	std::cerr << "menu" << std::endl;
	SDL_Event e;

	bool quit = false;

	while (!quit && runningGame)
	{
		SDL_Rect backButton = {20, 20, back.width, back.height};
		SDL_Rect classicButton = {(SCREEN_WIDTH - button.width) / 2, 450, button.width, button.height};
		SDL_Rect advanceButton = {(SCREEN_WIDTH - button.width) / 2, 600, button.width, button.height};

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			else if (insideHitbox(backButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					quit = true;
				}
			}
			else if (insideHitbox(classicButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);
				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					classicGamemode();
				}
			}

			else if (insideHitbox(advanceButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);
				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					advanceGamemode();
				}
			}
		}

		
		drawBackground();
		
		SDL_Color BLACK = {0, 0, 0, 255};

		window.render((SCREEN_WIDTH - frame.width) / 2, 100, frame.texture, NULL);
		window.render((SCREEN_WIDTH - frame.width) / 2 + 105, 100 + 55, gFontBig, "MENU", BLACK);

		window.render(20, 20, back.texture);

		window.render(classicButton.x, classicButton.y, button.texture);
		std::cerr << classicButton.x << ' ' << classicButton.y << ' ' << classicButton.w << ' ' << classicButton.h << std::endl;
		window.render(classicButton.x + 70, classicButton.y + 25, gFont28, "CLASSIC", BLACK);

		window.render(advanceButton.x, advanceButton.y, button.texture);
		window.render(advanceButton.x + 60, advanceButton.y + 25, gFont28, "ADVANCE", BLACK);

		window.display();
	}
}

void classicGamemode()
{
	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(cursor);

	std::cerr << "classic" << std::endl;
	mixer.stopPlayMenuSound();
	SDL_Event e;

	bool quit = false;
	
	bool keepPlaying = false;
	int gameMode = FRENZY;

	while (!quit && runningGame)
	{
		while (keepPlaying)
		{
			keepPlaying = chooseMode(gameMode);
		}

		SDL_Rect backButton = {20, 20, back.width, back.height};
		SDL_Rect enduranceButton = {(SCREEN_WIDTH - button.width) / 2, 350, button.width, button.height};
		SDL_Rect frenzyButton = {(SCREEN_WIDTH - button.width) / 2, 350 + 150, button.width, button.height};
		SDL_Rect patternButton = {(SCREEN_WIDTH - button.width) / 2, 350 + 150 * 2, button.width, button.height};

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			else if (insideHitbox(backButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					quit = true;
				}
			}
			else if (insideHitbox(enduranceButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					keepPlaying = chooseMode(ENDURANCE);
				}
			}
			else if (insideHitbox(frenzyButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					keepPlaying = chooseMode(FRENZY);
				}
			}
			else if (insideHitbox(patternButton))
			{
				cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
				SDL_SetCursor(cursor);

				if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					keepPlaying = chooseMode(PATTERN);
				}
			}
			
		}

		if (!keepPlaying)
		{
			drawBackground();
			
			SDL_Color BLACK = {0, 0, 0, 255};

			window.render((SCREEN_WIDTH - frame.width) / 2, 100, frame.texture, NULL);
			window.render((SCREEN_WIDTH - frame.width) / 2 + 20, 100 + 55, gFontBig, "CLASSIC", BLACK);

			window.render(20, 20, back.texture);

			window.render(enduranceButton.x, enduranceButton.y, button.texture);
			//std::cerr << menuButton.x << ' ' << menuButton.y << ' ' << menuButton.w << ' ' << menuButton.h << std::endl;
			window.render(enduranceButton.x + 40, enduranceButton.y + 25, gFont28, "ENDURANCE", BLACK);

			window.render(frenzyButton.x, frenzyButton.y, button.texture);
			window.render(frenzyButton.x + 80, frenzyButton.y + 25, gFont28, "FRENZY", BLACK);

			window.render(patternButton.x, patternButton.y, button.texture);
			window.render(patternButton.x + 65, patternButton.y + 25, gFont28, "PATTERN", BLACK);

			window.display();
		}
	}
}

void advanceGamemode()
{

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
						mixer.playWrongNoteSound();
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

	mixer.playWrongNoteSound();
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
						mixer.playWrongNoteSound();
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

	mixer.playWrongNoteSound();
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
