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
bool runningGame = true;

bool isPlayingClassic = true;

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
		std::cerr << path << std::endl;
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
	mixer.loadMenuGameSound("res/sfx/menu_sound_better.mp3");
	mixer.addRightNoteSound("res/sfx/click0_solved.wav");
	mixer.addRightNoteSound("res/sfx/click2_solved.wav");
	mixer.addRightNoteSound("res/sfx/click3_solved.wav");
	mixer.loadWrongNoteound("res/sfx/failed01.wav");
}

//Draw background, slightly move down for each frame
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
	SDL_Event e;

	bool quit = false;

	while (!quit && runningGame)
	{
		SDL_Rect menuButton = {(SCREEN_WIDTH - button.width) / 2, 450, button.width, button.height};
		SDL_Rect aboutUsButton = {(SCREEN_WIDTH - button.width) / 2, 600, button.width, button.height};

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
			{
				//Close game
				runningGame = false;
			}
			else if (insideHitbox(menuButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				//Start the "Menu" screen
				menu();
			}
			else if (insideHitbox(aboutUsButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				//Start the "About Us" screen
				aboutUs();
			}
		}

		drawBackground();

		window.render((SCREEN_WIDTH - logoName.width) / 2, 50, logoName.texture);

		SDL_Color BLACK = {0, 0, 0, 255};

		window.render(menuButton.x, menuButton.y, button.texture);
		window.render(menuButton.x + 95, menuButton.y + 25, gFont28, "MENU", BLACK);

		window.render(aboutUsButton.x, aboutUsButton.y, button.texture);
		window.render(menuButton.x + 62, menuButton.y + 175, gFont28, "ABOUT US", BLACK);

		window.display();
	}
}

void aboutUs()
{
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
			else if (e.type == SDL_MOUSEBUTTONDOWN && insideHitbox(backButton))
			{
				quit = true;
			}
		}

		drawBackground();

		window.render((SCREEN_WIDTH - groupName.width) / 2, 12, groupName.texture);

		window.render(0, 44, avatarGroup);
		
		window.render(20, 20, back.texture);

		window.display();
	}
}

void menu()
{
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
			else if (insideHitbox(backButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				quit = true;
			}
			else if (insideHitbox(classicButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				classicGamemode();
			}

			else if (insideHitbox(advanceButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				advanceGamemode();
			}
		}

		
		drawBackground();
		
		SDL_Color BLACK = {0, 0, 0, 255};

		window.render((SCREEN_WIDTH - frame.width) / 2, 100, frame.texture, NULL);
		window.render((SCREEN_WIDTH - frame.width) / 2 + 150, 100 + 70, gFontBig, "MENU", BLACK);

		window.render(20, 20, back.texture);

		window.render(classicButton.x, classicButton.y, button.texture);
		//std::cerr << classicButton.x << ' ' << classicButton.y << ' ' << classicButton.w << ' ' << classicButton.h << std::endl;
		window.render(classicButton.x + 70, classicButton.y + 25, gFont28, "CLASSIC", BLACK);

		window.render(advanceButton.x, advanceButton.y, button.texture);
		window.render(advanceButton.x + 67, advanceButton.y + 25, gFont28, "ADVANCE", BLACK);

		window.display();
	}
}

void classicGamemode()
{
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
		SDL_Rect enduranceButton = {(SCREEN_WIDTH - button.width) / 2, 400, button.width, button.height};
		SDL_Rect frenzyButton = {(SCREEN_WIDTH - button.width) / 2, enduranceButton.y + 150, button.width, button.height};
		SDL_Rect patternButton = {(SCREEN_WIDTH - button.width) / 2, enduranceButton.y + 150 * 2, button.width, button.height};

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			else if (insideHitbox(backButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				quit = true;
			}
			else if (insideHitbox(enduranceButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				gameMode = ENDURANCE;
				isPlayingClassic = true;
				keepPlaying = chooseMode(ENDURANCE);
			}
			else if (insideHitbox(frenzyButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				gameMode = FRENZY;
				isPlayingClassic = true;
				keepPlaying = chooseMode(FRENZY);
				
			}
			else if (insideHitbox(patternButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				gameMode = PATTERN;
				isPlayingClassic = true;
				keepPlaying = chooseMode(PATTERN);
			}
			
		}

		if (!keepPlaying)
		{
			drawBackground();
			
			SDL_Color BLACK = {0, 0, 0, 255};

			window.render((SCREEN_WIDTH - frame.width) / 2, 100, frame.texture, NULL);
			window.render((SCREEN_WIDTH - frame.width) / 2 + 80, 100 + 75, gFontBig, "CLASSIC", BLACK);

			window.render(20, 20, back.texture);

			window.render(enduranceButton.x, enduranceButton.y, button.texture);
			window.render(enduranceButton.x + 45, enduranceButton.y + 25, gFont28, "ENDURANCE", BLACK);

			window.render(frenzyButton.x, frenzyButton.y, button.texture);
			window.render(frenzyButton.x + 80, frenzyButton.y + 25, gFont28, "FRENZY", BLACK);

			window.render(patternButton.x, patternButton.y, button.texture);
			window.render(patternButton.x + 70, patternButton.y + 25, gFont28, "PATTERN", BLACK);

			window.display();
		}
	}
}

void advanceGamemode()
{
	SDL_Event e;

	bool quit = false;
	
	bool keepPlaying = false;
	int gameMode = FRENZY;
	int sizeGrid = 4;
	Uint64 timeLimit = 30 * 1000;
	int numBlack = 3;

	while (!quit && runningGame)
	{
		while (keepPlaying)
		{
			keepPlaying = chooseMode(gameMode, sizeGrid, timeLimit, numBlack);
		}

		SDL_Rect backButton = {20, 20, back.width, back.height};
		SDL_Rect optionButton = {(SCREEN_WIDTH - button.width) / 2, 450, button.width, button.height};
		SDL_Rect randomButton = {(SCREEN_WIDTH - button.width) / 2, 600, button.width, button.height};
		
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
			}
			else if (insideHitbox(backButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				quit = true;
			}
			
			else if (insideHitbox(optionButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (optionMap(gameMode, sizeGrid, timeLimit))
				{
					isPlayingClassic = false;
					keepPlaying = chooseMode(gameMode, sizeGrid, timeLimit, numBlack);
				}
			}
			else if (insideHitbox(randomButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				gameMode = (rand() % 3);
				sizeGrid = 4 + (rand() % 4);
				timeLimit = (1 + (rand() % 4)) * 15 * 1000;
				numBlack = 3 + (rand() % 4);
				isPlayingClassic = false;
				keepPlaying = chooseMode(gameMode, sizeGrid, timeLimit, numBlack);
			}
		}

		if (!keepPlaying)
		{
			drawBackground();
			
			SDL_Color BLACK = {0, 0, 0, 255};

			window.render((SCREEN_WIDTH - frame.width) / 2, 100, frame.texture, NULL);
			window.render((SCREEN_WIDTH - frame.width) / 2 + 45, 100 + 75, gFontBig, "ADVANCE", BLACK);

			window.render(20, 20, back.texture);

			window.render(optionButton.x, optionButton.y, button.texture);
			window.render(optionButton.x + 77, optionButton.y + 25, gFont28, "OPTION", BLACK);

			window.render(randomButton.x, randomButton.y, button.texture);
			window.render(randomButton.x + 75, randomButton.y + 25, gFont28, "RANDOM", BLACK);

			window.display();
		}
	}
}

bool optionMap(int &gamemode, int &sizeGrid, Uint64 &timeLimit)
{
	SDL_Event e;

	bool quit = false;
	bool startGame = true;
	int choosedGamemode = -1;
	int choosedSize = -1;
	int choosedTime = -1;
	
	while (!quit && runningGame)
	{
		SDL_Rect backButton = {20, 20, back.width, back.height};
		SDL_Rect finishButton = {229, 656, finish.width, finish.height};
		SDL_Rect gameOption[3];
			gameOption[0] = {9, 224, big_round.width, big_round.height};
			gameOption[1] = {9, 328, big_round.width, big_round.height};
			gameOption[2] = {9, 430, big_round.width, big_round.height};
		SDL_Rect sizeOption[4];
			sizeOption[0] = {350, 224, small_round.width, small_round.height};
			sizeOption[1] = {350, 328, small_round.width, small_round.height};
			sizeOption[2] = {350, 430, small_round.width, small_round.height};
			sizeOption[3] = {350, 532, small_round.width, small_round.height};
		SDL_Rect timeOption[4];
			timeOption[0] = {620, 224, small_round.width, small_round.height};
			timeOption[1] = {620, 328, small_round.width, small_round.height};
			timeOption[2] = {620, 430, small_round.width, small_round.height};
			timeOption[3] = {620, 532, small_round.width, small_round.height};

		auto insideGroupRect = [&](SDL_Rect groupRect[], int len) ->bool {
			for (int i = 0; i < len; i++)
				if (insideHitbox(groupRect[i]))
					return true;
			return false;
		};

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
				startGame = false;
			}
			else if (insideHitbox(backButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				quit = true;
				startGame = false;
			}
			else if (insideGroupRect(gameOption, 3) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				for (int i = 0; i < 3; i++)
				{
					if (insideHitbox(gameOption[i]))
					{
						choosedGamemode = i;
						break;
					}
				}
			}
			else if (insideGroupRect(sizeOption, 4) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				for (int i = 0; i < 4; i++)
				{
					if (insideHitbox(sizeOption[i]))
					{
						choosedSize = i;
						break;
					}
				}
			}
			else if (insideGroupRect(timeOption, 4) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				for (int i = 0; i < 4; i++)
				{
					if (insideHitbox(timeOption[i]))
					{
						choosedTime = i;
						break;
					}
				}
			}
			else if (insideHitbox(finishButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (choosedGamemode >= 0 && choosedSize >= 0 && choosedTime >= 0)
				{
					gamemode = choosedGamemode;
					sizeGrid = choosedSize + 4;
					timeLimit = (choosedTime + 1) * 15 * 1000;
					quit = true;
					startGame = true;
				}
			}
		}
		
		drawBackground();
		//std::cerr << choosedGamemode << ' ' << choosedSize << ' ' << choosedTime << std::endl;

		std::stringstream tempStream;

		SDL_Color BLACK = {0, 0, 0, 255};
		SDL_Color WHITE = {255, 255, 255, 255};
		//Gamemode options
		window.render(9, 109, big_frame.texture);
		window.render(9 + 50, 109 + 15, gFontMedium, "MODE", BLACK);
		window.render(gameOption[0].x, gameOption[0].y, big_round.texture);
		window.render(gameOption[0].x + 12, gameOption[0].y + 22, gFont35, "ENDURANCE", BLACK);
		window.render(gameOption[1].x, gameOption[1].y, big_round.texture);
		window.render(gameOption[1].x + 55, gameOption[1].y + 22, gFont35, "FRENZY", BLACK);
		window.render(gameOption[2].x, gameOption[2].y, big_round.texture);
		window.render(gameOption[2].x + 40, gameOption[2].y + 22, gFont35, "PATTERN", BLACK);
		if (choosedGamemode >= 0) 
		{
			std::string content = "";
			int dis = 0;
			switch (choosedGamemode)
			{
				case 0:
					content = "ENDURANCE";
					dis = 12;
					break;
				case 1:
					content = "FRENZY";
					dis = 55;
					break;
				default:
					content = "PATTERN";
					dis = 40;
					break;
			}
			window.render(gameOption[choosedGamemode].x, gameOption[choosedGamemode].y, big_round_black.texture);
			window.render(gameOption[choosedGamemode].x + dis, gameOption[choosedGamemode].y + 22, gFont35, content, WHITE);
		}

		//Size options
		window.render(275, 109, big_frame.texture);
		window.render(275 + 57, 109 + 15, gFontMedium, "SIZE", BLACK);
		for (int i = 0; i < 4; i++)
		{
			window.render(sizeOption[i].x, sizeOption[i].y, small_round.texture);
			tempStream.str("");
			tempStream << i + 4 << 'x' << i + 4;
			window.render(sizeOption[i].x + 12, sizeOption[i].y + 22, gFont35, tempStream.str().c_str(), BLACK);
		}
		if (choosedSize >= 0)
		{
			window.render(sizeOption[choosedSize].x, sizeOption[choosedSize].y, small_round_black.texture);
			tempStream.str("");
			tempStream << choosedSize + 4 << 'x' << choosedSize + 4;
			window.render(sizeOption[choosedSize].x + 12, sizeOption[choosedSize].y + 22, gFont35, tempStream.str().c_str(), WHITE);
		}

		//Time options
		window.render(541, 109, big_frame.texture);
		window.render(541 + 57, 109 + 15, gFontMedium, "TIME", BLACK);
		for (int i = 0; i < 4; i++)
		{
			window.render(timeOption[i].x, timeOption[i].y, small_round.texture);
			tempStream.str("");
			tempStream << (i + 1) * 15;
			window.render(timeOption[i].x + 28, timeOption[i].y + 22, gFont35, tempStream.str().c_str(), BLACK);
		}
		if (choosedTime >= 0)
		{
			tempStream.str("");
			tempStream << (choosedTime + 1) * 15;

			window.render(timeOption[choosedTime].x, timeOption[choosedTime].y, small_round_black.texture);
			window.render(timeOption[choosedTime].x + 28, timeOption[choosedTime].y + 22, gFont35, tempStream.str().c_str(), WHITE);
		}

		window.render(20, 20, back.texture);

		window.render(finishButton.x, finishButton.y, finish.texture);

		window.display();
		
	}
	return startGame;
}

bool waitingForStart(int gamemode)
{
	Uint64 timeStart = SDL_GetTicks64();
	SDL_Color BLACK = {0, 0, 0, 255};

	SDL_Event e;

	while (true)
	{
		while(SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				runningGame = false;
				return false;
			}
		}

		window.cleanScreen();
		Uint64 amountTime = SDL_GetTicks64() - timeStart;
		if (amountTime <= 3000)
		{
			if (gamemode == ENDURANCE)
			{
				window.render(300, 200, gFontMedium, "Play\nas\nlong\nas\npossible", BLACK);
			}
			else if (gamemode == FRENZY)
			{
				window.render(300, 150, gFontMedium, "Get\nthe\nhighest\nscore\nin\nlimited\ntime", BLACK);
			}
			else 
			{
				assert(gamemode == PATTERN);
				window.render(300, 150, gFontMedium, "Clear\nall\npattern\nas\nfast\nas\npossible", BLACK);
			}
		}
		else if (amountTime <= 3000 + 1000)
		{
			window.render(325, 350, gFontBigger, "3", BLACK);
		}
		else if (amountTime <= 3000 + 1000 * 2)
		{
			window.render(325, 350, gFontBigger, "2", BLACK);
		}
		else if (amountTime <= 3000 + 1000 * 3)
		{
			window.render(325, 350, gFontBigger, "1", BLACK);
		}
		else
		{
			break;
		}

		window.display();
	}

	return true;
}

bool chooseMode(int gamemode, int sizeGrid, Uint64 timeLimit, int numBlack)
{
	if (numBlack == 0)
	{
		numBlack = 3 + (sizeGrid >= 6);
	}
	mixer.lowerPlayMenuSound();
	if (!waitingForStart(gamemode))
	{
		return false;
	}
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

	const int amountTimeBonus = timeLimit;
	int score = 0;
	int requiredScore = std::min(numBlack * (1 + amountTimeBonus / 1000), 40) * timeLimit / 10000;
	
	Uint64 timeStart = SDL_GetTicks64();
	Uint64 timeBonus = 0;

	std::vector<std::pair<std::array<int, 4>, Uint64>>queryRender;

	std::stringstream textStream;
	SDL_Color colorText = {0, 0, 0, 255};

	//std::cerr << "start" << std::endl;

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
						currentGrid.changeColor(coorCell.first, coorCell.second);
						int row = coorCell.first, col = coorCell.second;
						int x = currentGrid.getCoor(row, col).x, y = currentGrid.getCoor(row, col).y;

						std::array<int, 4> tmp = {x, y, 1, 100};
						queryRender.emplace_back(tmp, SDL_GetTicks64());

						if (++score % requiredScore == 0)
						{
							timeBonus += amountTimeBonus;
						}
						if (score == INT_MAX)
						{
							quit = true;
						}
					}
					else
					{
						//std::cerr << "wrong note" << std::endl;
						int row = coorCell.first, col = coorCell.second;
						mixer.playWrongNoteSound();

						SDL_Color RED = {255, 0, 0, 0};
						int startTime = SDL_GetTicks64();
						while(SDL_GetTicks64() - startTime <= 1000)
						{
							//std::cerr << "render red" << std::endl;
							window.renderTitle(currentGrid.getCell(row, col), 255, 0, 0, 255);
							window.display();
						}

						quit = true;
						
					}
				}
			}
		}

		if (SDL_GetTicks64() - timeStart >= timeLimit + timeBonus)
		{
			quit = true;
			break;
		}

		window.renderGrid(currentGrid);

		int sx = currentGrid.getGridRect()->x;
		int sy = currentGrid.getGridRect()->y;
		int len = currentGrid.getGridRect()->w;
		SDL_Color BLACK = {0, 0, 0, 255};
		SDL_Color RED = {255, 0, 0, 255};

		for (auto &[info, startTime]: queryRender)
		{
			if (SDL_GetTicks64() - startTime > info[3])
			{
				continue;
			}
			else 
			{
				window.render(info[0], info[1], point[info[2]].texture);
			}
		}
		
		window.render(sx, sy - 120, gFont35, "TIME", BLACK, 1, sx, sx + len);

		textStream.str("");
		textStream << (timeLimit + timeBonus - SDL_GetTicks64() + timeStart) / 1000;
		window.render(sx, sy - 72, gFont70, textStream.str().c_str(), RED, 1, sx, sx + len);

		window.render(sx, sy - 120, gFont35, "SCORE", BLACK, 2, sx, sx + len);

		textStream.str("");
		textStream << score;
		window.render(sx, sy - 72, gFont70, textStream.str().c_str(), RED, 2, sx, sx + len);

		window.display();
	}

	return getResponse(ENDURANCE, score);
}

bool frenzyMode(int sizeGrid, Uint64 timeLimit, int numBlack)
{
	Grid currentGrid = Grid(sizeGrid, timeLimit, numBlack, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Event e;

	bool quit = false;

	int score = 0;
	int numHit = 0;
	int multiply = 1;

	Uint64 timeStart = SDL_GetTicks64();
	std::vector<std::pair<std::array<int, 4>, Uint64>>queryRender;

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
						mixer.playRightNoteSound();
						int row = coorCell.first, col = coorCell.second;
						int x = currentGrid.getCoor(row, col).x, y = currentGrid.getCoor(row, col).y;

						std::array<int, 4> tmp = {x, y, multiply, 100};
						queryRender.emplace_back(tmp, SDL_GetTicks64());

						currentGrid.changeColor(coorCell.first, coorCell.second);
						score += multiply;
						numHit++;
					}
					else
					{
						int row = coorCell.first, col = coorCell.second;
						mixer.playWrongNoteSound();

						SDL_Color RED = {255, 0, 0, 0};
						int startTime = SDL_GetTicks64();
						while(SDL_GetTicks64() - startTime <= 1000)
						{
							std::cerr << "render red" << std::endl;
							window.renderTitle(currentGrid.getCell(row, col), 255, 0, 0, 255);
							window.display();
						}

						quit = true;
					}
				}
			}
		}

		if (SDL_GetTicks64() - timeStart >= timeLimit)
		{
			quit = true;
		}

		int average = 1000 * numHit / (SDL_GetTicks64() - timeStart + 1);

		multiply = std::min(5, average / 2 + (average < 2));

		window.renderGrid(currentGrid);

		int sx = currentGrid.getGridRect()->x;
		int sy = currentGrid.getGridRect()->y;
		int len = currentGrid.getGridRect()->w;
		SDL_Color BLACK = {0, 0, 0, 255};
		SDL_Color RED = {255, 0, 0, 255};

		for (auto &[info, startTime]: queryRender)
		{
			if (SDL_GetTicks64() - startTime > info[3])
			{
				continue;
			}
			else 
			{
				window.render(info[0], info[1], point[info[2]].texture);
			}
		}
		
		window.render(sx, sy - 120, gFont35, "TIME", BLACK, 1, sx, sx + len);

		textStream.str("");
		textStream << (timeLimit - SDL_GetTicks64() + timeStart) / 1000;
		window.render(sx, sy - 72, gFont70, textStream.str().c_str(), RED, 1, sx, sx + len);

		if (sizeGrid < 5)
		{
			window.render(sx, sy + len + 5, gFont35, "MULTIPLY", BLACK, 3, sx, sx + len);

			textStream.str("");
			textStream << multiply;
			window.render(sx, sy + len + 45, gFont70, textStream.str().c_str(), RED, 3, sx, sx + len);
		}
		else
		{
			window.render(sx, sy - 120, gFont35, "MULTIPLY", BLACK, 3, sx, sx + len);

			textStream.str("");
			textStream << multiply;
			window.render(sx, sy - 72, gFont70, textStream.str().c_str(), RED, 3, sx, sx + len);

		}

		window.render(sx, sy - 120, gFont35, "SCORE", BLACK, 2, sx, sx + len);

		textStream.str("");
		textStream << score;
		window.render(sx, sy - 72, gFont70, textStream.str().c_str(), RED, 2, sx, sx + len);

		window.display();
	}

	window.cleanScreen();
	textStream.str("");
	textStream << "Your score: " << score << ". Press R to play again or T to choose another gamemode";
	window.render(0, 0, gFont28, textStream.str().c_str(), colorText);
	window.display();

	return getResponse(FRENZY, score);
}

bool patternMode(int sizeGrid, Uint64 timeLimit, int numBlack)
{
	Grid currentGrid = Grid(sizeGrid, timeLimit, numBlack, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Event e;

	bool quit = false;

	int numPattern = timeLimit / 1000;
	int leftBlack = numBlack;

	Uint64 timeStart = SDL_GetTicks64();
	std::vector<std::pair<std::array<int, 4>, Uint64>>queryRender;

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
						int row = coorCell.first, col = coorCell.second;
						int x = currentGrid.getCoor(row, col).x, y = currentGrid.getCoor(row, col).y;

						std::array<int, 4> tmp = {row, col, 1, 100};
						queryRender.emplace_back(tmp, SDL_GetTicks64());

						currentGrid.onlyChangeColor(coorCell.first, coorCell.second);
						leftBlack--;
					}
					else
					{
						quit = true;
						int row = coorCell.first, col = coorCell.second;
						mixer.playWrongNoteSound();

						SDL_Color RED = {255, 0, 0, 0};
						int startTime = SDL_GetTicks64();
						while(SDL_GetTicks64() - startTime <= 1000)
						{
							std::cerr << "render red" << std::endl;
							window.renderTitle(currentGrid.getCell(row, col), 255, 0, 0, 255);
							window.display();
						}
					}
				}
			}
		}

		if ((numPattern == 1 && leftBlack == 0) || SDL_GetTicks64() - timeStart >= timeLimit)
		{
			quit = true;
		}
		else 
		{
			if (leftBlack == 0)
			{
				currentGrid.generateBlackCell();
				leftBlack = numBlack;
				numPattern--;
			}
			window.renderGrid(currentGrid);
		}
		int sx = currentGrid.getGridRect()->x;
		int sy = currentGrid.getGridRect()->y;
		int len = currentGrid.getGridRect()->w;
		SDL_Color BLACK = {0, 0, 0, 255};
		SDL_Color RED = {255, 0, 0, 255};

		for (auto &[info, startTime]: queryRender)
		{
			if (SDL_GetTicks64() - startTime > info[3])
			{
				continue;
			}
			else 
			{
				window.renderTitle(currentGrid.getCell(info[0], info[1]), 0, 255, 0, 255);
			}
		}
		
		window.render(sx, sy - 120, gFont35, "TIME", BLACK, 1, sx, sx + len);

		textStream.str("");
		textStream << (timeLimit - SDL_GetTicks64() + timeStart) / 1000;
		window.render(sx, sy - 72, gFont70, textStream.str().c_str(), RED, 1, sx, sx + len);

		window.render(sx, sy - 120, gFont35, "PATTERN", BLACK, 2, sx, sx + len);

		textStream.str("");
		textStream << numPattern;
		window.render(sx, sy - 72, gFont70, textStream.str().c_str(), RED, 2, sx, sx + len);

		window.display();
	}

	return getResponse(PATTERN, (numPattern == 1 && leftBlack == 0) * (SDL_GetTicks64() - timeStart));
}

bool getResponse(int gamemode, int currentScore)
{
	if (isPlayingClassic)
	{
		highscore[gamemode].push_back(currentScore);
		if (gamemode == PATTERN)
		{
			std::sort(highscore[gamemode].begin(), highscore[gamemode].end());
		}
		else
		{
			std::sort(highscore[gamemode].begin(), highscore[gamemode].end(), std::greater<int>());
		}

		if (highscore[gamemode].size() > 5)
		{
			highscore[gamemode].pop_back();
		}
	}

	SDL_Rect replayButton = {214 + (SCREEN_WIDTH - result.width) / 2, (SCREEN_HEIGHT + result.height) / 2 - 70, replay.width, replay.height};
	SDL_Rect homeButton = {284 + (SCREEN_WIDTH - result.width) / 2, (SCREEN_HEIGHT + result.height) / 2 - 70, home.width, home.height};

	std::stringstream tempStream;

	window.cleanScreen();

	window.render((SCREEN_WIDTH - result.width) / 2, (SCREEN_HEIGHT - result.height) / 2, result.texture);

	SDL_Color RED = {255, 0, 0, 255};

	tempStream.str("");
	tempStream << currentScore;
	window.render((SCREEN_WIDTH - result.width) / 2 + result.width / 2 + 40, (SCREEN_HEIGHT - result.height) / 2 + 80, gFontMedium,
	tempStream.str().c_str(), RED);

	window.display();

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
			else if (insideHitbox(replayButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				return true;
			}
			else if (insideHitbox(homeButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				return false;
			}
		}
	}
	return true;
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

int main( int argc, char* args[] ){
	srand(time(NULL));   

    init();

	loadMedia();

	startGame();

	close();

	return EXIT_SUCCESS;
}