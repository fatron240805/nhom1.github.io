#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <chrono>
#include <random>
#include <assert.h>

#include "Title.h"

const int DISTANCE_CELL = 2; // thickness of grid edge

class Grid // manage the grid's properties
{
    public:
        Grid(); // initialize

        ~Grid(); // free the memory 

        Grid(int __sizeGrid, Uint64 __timeLimit, int __numBlack = 3, int SCREEN_WIDTH = 1280, int SCREEN_HEIGHT = 720); // initialize the grid with it's properties

        void generateBlackCell(); // randomly generate black cells

        void onlyChangeColor(int row, int col);  // change color of cell only

        void changeColor(int row, int col); // change the color and state of cell

        int getSize(); // get size of the grid
        
        Title* getCell(int row, int col); // get the cell in row (row) and column (col) of grid to a pointer Title*

        bool getColor(int row, int col); // get the color of the cell in row (row) and column (col)

        SDL_Point getCoor(int row, int col); // get the positionStart of cell in row (row) and column (col)

        SDL_Rect* getGridRect(); // get the gridRect of grid

        //unoptimized
        std::pair<int, int> identifyCell(); // find the position (row, col) of the cell that the mouse is pointing

        bool validAction(std::pair<int, int> coorCell); // check if the mouse click action is correct or not
        
        void debug(); // for debugging 

    private:
        int sizeGrid; // size of the grid (= 5 if the grid is 5x5)
        int numberBlack; // number of black-cell
        Uint64 timeLimit; // time limit
        SDL_Rect gridRect; // an object stores information about the frame surrounding the grid
        std::vector<bool>currentGridColor; // a dynamic array store the color of all the cell of grid (0-white and 1-black)
        std::vector<std::vector<Title>>currentGrid; // a 2D dynamic array, each element is a Title object stores information about a cell
};

