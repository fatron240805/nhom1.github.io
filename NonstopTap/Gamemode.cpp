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
				window.render(300, 200, gFontMedium, "Play\nas\nlong\nas\npossible", BLACK, 3, 0, SCREEN_WIDTH);
			}
			else if (gamemode == FRENZY)
			{
				window.render(300, 150, gFontMedium, "Get\nthe\nhighest\nscore\nin\nlimited\ntime", BLACK, 3, 0, SCREEN_WIDTH);
			}
			else 
			{
				assert(gamemode == PATTERN);
				window.render(300, 150, gFontMedium, "Clear\nall\npattern\nas\nfast\nas\npossible", BLACK, 3, 0, SCREEN_WIDTH);
			}
		}
		else if (amountTime <= 3000 + 1000)
		{
			window.render(300, 300, gFontBigger, "3", BLACK, 3, 0, SCREEN_WIDTH);
		}
		else if (amountTime <= 3000 + 1000 * 2)
		{
			window.render(300, 300, gFontBigger, "2", BLACK, 3, 0, SCREEN_WIDTH);
		}
		else if (amountTime <= 3000 + 1000 * 3)
		{
			window.render(300, 300, gFontBigger, "1", BLACK, 3, 0, SCREEN_WIDTH);
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

	const int amountTimeBonus = 10000;
	int score = 0;
	int requiredScore = std::max(numBlack * (8 + amountTimeBonus / 15000), 40);
	
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
			mixer.playWrongNoteSound();
			while(SDL_GetTicks64() - timeStart <= timeLimit + timeBonus + 1000) {}
			quit = true;
			break;
		}

		window.renderGrid(currentGrid);

		int sx = currentGrid.getGridRect()->x;
		int sy = currentGrid.getGridRect()->y;
		int len = currentGrid.getGridRect()->w;
		SDL_Color BLACK = {0, 0, 0, 255};
		SDL_Color RED = {255, 0, 0, 255};

		std::sort(queryRender.begin(), queryRender.end(), [&](std::pair<std::array<int, 4>, Uint64> T1, std::pair<std::array<int, 4>, Uint64> T2) { return T1.second >= T2.second; });

		while(!queryRender.empty() && SDL_GetTicks64() - queryRender.back().second > queryRender.back().first[3])
		{
			queryRender.pop_back();
		}

		for (auto &[info, startTime]: queryRender)
		{
			window.render(info[0], info[1], point[info[2]].texture);
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
			mixer.playWrongNoteSound();
			while(SDL_GetTicks64() - timeStart <= timeLimit + 1000) {}
			quit = true;
		}

		int average = 1000 * numHit / (SDL_GetTicks64() - timeStart + 1);

		multiply = std::min(5, 1 + (average - 3) * (average > 3));

		window.renderGrid(currentGrid);

		int sx = currentGrid.getGridRect()->x;
		int sy = currentGrid.getGridRect()->y;
		int len = currentGrid.getGridRect()->w;
		SDL_Color BLACK = {0, 0, 0, 255};
		SDL_Color RED = {255, 0, 0, 255};

		std::sort(queryRender.begin(), queryRender.end(), [&](std::pair<std::array<int, 4>, Uint64> T1, std::pair<std::array<int, 4>, Uint64> T2) { return T1.second >= T2.second; });

		while(!queryRender.empty() && SDL_GetTicks64() - queryRender.back().second > queryRender.back().first[3])
		{
			queryRender.pop_back();
		}

		for (auto &[info, startTime]: queryRender)
		{
			window.render(info[0], info[1], point[info[2]].texture);
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

	int numPattern = (int)(timeLimit / 2000);
	int leftBlack = numBlack;

	Uint64 timeStart = SDL_GetTicks64();
	std::vector<std::pair<std::array<int, 4>, Uint64>>queryRender;

	std::stringstream textStream;
	SDL_Color colorText = {0, 0, 0, 255};

	window.renderGrid(currentGrid);

	int lastRow = -1, lastCol = -1;
	bool completed = false;

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
						lastRow = row, lastCol = col;
						int x = currentGrid.getCoor(row, col).x, y = currentGrid.getCoor(row, col).y;

						std::array<int, 4> tmp = {row, col, 1, 100};
						queryRender.emplace_back(tmp, SDL_GetTicks64());

						currentGrid.onlyChangeColor(coorCell.first, coorCell.second);
						leftBlack--;

						if (numPattern == 1 && leftBlack == 0)
						{
							completed = true;
						}
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
							//std::cerr << "render red" << std::endl;
							window.renderTitle(currentGrid.getCell(row, col), 255, 0, 0, 255);
							window.display();
						}
					}
				}
			}
		}

		if (completed || SDL_GetTicks64() - timeStart >= timeLimit)
		{
			//std::cerr << "Clear!" << std::endl;
			Uint64 newTimeStart = SDL_GetTicks64();
			if (!(numPattern == 1 && leftBlack == 0))
			{
				mixer.playWrongNoteSound();
				while(SDL_GetTicks64() - newTimeStart <= 1000) {}
			}
			else
			{
				while(SDL_GetTicks64() - newTimeStart <= 100) 
				{
					window.renderTitle(currentGrid.getCell(lastRow, lastCol), 0, 255, 0, 255);
					window.display();
				}
				while(SDL_GetTicks64() - newTimeStart <= 1000) 
				{
					window.renderTitle(currentGrid.getCell(lastRow, lastCol), 255, 255, 255, 255);
					window.display();
				}
			}
			quit = true;
			if (completed)
			{
				break;
			}
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
		//std::cerr << "test1" << ' ' << quit << std::endl;

		std::sort(queryRender.begin(), queryRender.end(), [&](std::pair<std::array<int, 4>, Uint64> T1, std::pair<std::array<int, 4>, Uint64> T2) { return T1.second >= T2.second; });

		while(!queryRender.empty() && SDL_GetTicks64() - queryRender.back().second > queryRender.back().first[3])
		{
			queryRender.pop_back();
		}

		for (auto &[info, startTime]: queryRender)
		{
			window.renderTitle(currentGrid.getCell(info[0], info[1]), 0, 255, 0, 255);
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
		//std::cerr << quit << std::endl;
	}

	int result = (numPattern == 1 && leftBlack == 0 ? (SDL_GetTicks64() - timeStart) : INT_MAX);

	return getResponse(PATTERN, result);
}

bool getResponse(int gamemode, int currentScore)
{
	mixer.stopPlayMenuSound();
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
	if (currentScore < INT_MAX)
	{
		tempStream << (gamemode != PATTERN ? currentScore : 1.0 * currentScore / 1000);
	}
	window.render((SCREEN_WIDTH - result.width) / 2 + result.width / 2 + 40, (SCREEN_HEIGHT - result.height) / 2 + 36, gFontMedium, tempStream.str().c_str(), RED);

	if (isPlayingClassic)
	{
		for (int i = 0; i < 5; i++)
		{
			tempStream.str("");
			if (highscore[gamemode].size() > i && highscore[gamemode][i] < INT_MAX)
			{
				tempStream << (gamemode != PATTERN ? highscore[gamemode][i] : 1.0 * highscore[gamemode][i] / 1000);
			}
			window.render((SCREEN_WIDTH - result.width) / 2 + result.width / 2 - 180 + 40, (SCREEN_HEIGHT - result.height) / 2 + 36 + 168 + i * 61, gFontMedium,
			tempStream.str().c_str(), RED);
		}
	}

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
				mixer.stopAnySound();
				mixer.lowerPlayMenuSound();
				mixer.playMenuSound();
				return true;
			}
			else if (insideHitbox(homeButton) && e.type == SDL_MOUSEBUTTONDOWN)
			{
				mixer.stopAnySound();
				mixer.resetPlayMenuSound();
				mixer.playMenuSound();
				return false;
			}
		}
	}
	return true;
}