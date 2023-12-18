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

//Draw background, slightly move down for each frame
void drawBackground()
{
	static int frame = 0;

	//std::cerr << frame << std::endl;

	mixer.playMenuSound(); //play the menu-music
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
		// create hitbox for MENU and ABOUT US button
		SDL_Rect menuButton = {(SCREEN_WIDTH - button.width) / 2, 450, button.width, button.height};
		SDL_Rect aboutUsButton = {(SCREEN_WIDTH - button.width) / 2, 600, button.width, button.height};

		// handle the input events from keyboard and mouse
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

		// render MENU and ABOUT US button
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
		// create hibox for back, classic and advance button
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

		// render image (the text "MENU", classic and advance button)
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
	
	bool keepPlaying = false; // check if player keep playing the previous gamemode
	int gameMode = FRENZY; // 0 ~ endurance, 1 ~ frenzy, 2 ~ pattern

	while (!quit && runningGame)
	{
		while (keepPlaying)
		{
			keepPlaying = chooseMode(gameMode);
		}

		// create hitbox for button
		SDL_Rect backButton = {20, 20, back.width, back.height};
		SDL_Rect enduranceButton = {(SCREEN_WIDTH - button.width) / 2, 400, button.width, button.height};
		SDL_Rect frenzyButton = {(SCREEN_WIDTH - button.width) / 2, enduranceButton.y + 150, button.width, button.height};
		SDL_Rect patternButton = {(SCREEN_WIDTH - button.width) / 2, enduranceButton.y + 150 * 2, button.width, button.height};

		// handle the input events from keyboard and mouse
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
			// if 
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
		
		//create back, option and random button hitbox
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
		// render hitbox for button
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

		// handle the input events from keyboard and mouse
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