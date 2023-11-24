#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <chrono>
#include <random>
#include <assert.h>

#include "Title.h"

const int DISTANCE_CELL = 2;

class Grid
{
    public:
        Grid();

        ~Grid();

        Grid(int __sizeGrid, Uint64 __timeLimit, int __numBlack = 3, int SCREEN_WIDTH = 1280, int SCREEN_HEIGHT = 720);

        void generateBlackCell();

        void onlyChangeColor(int row, int col);

        void changeColor(int row, int col);

        int getSize();
        
        Title* getCell(int row, int col);

        bool getColor(int row, int col);

        SDL_Point getCoor(int row, int col);

        SDL_Rect* getGridRect();

        //unoptimized
        std::pair<int, int> identifyCell();

        bool validAction(std::pair<int, int> coorCell);
        
        void debug();

    private:
        int sizeGrid;
        int numberBlack;
        Uint64 timeLimit;
        SDL_Rect gridRect;
        std::vector<bool>currentGridColor;
        std::vector<std::vector<Title>>currentGrid;
};

