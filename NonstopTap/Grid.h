#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <chrono>
#include <random>
#include <assert.h>

#include "Title.h"

// thickness of grid edge
const int DISTANCE_CELL = 2; 

// manage the grid's properties
class Grid 
{
    public:
        // initialize
        Grid(); 

        // free the memory 
        ~Grid(); 
        
        // initialize the grid with it's properties
        Grid(int __sizeGrid, Uint64 __timeLimit, int __numBlack = 3, int SCREEN_WIDTH = 1280, int SCREEN_HEIGHT = 720); 
    
        // randomly generate black cells
        void generateBlackCell(); 

        // change color of cell only
        void onlyChangeColor(int row, int col);  
        
        // change the color and state of cell
        void changeColor(int row, int col); 
      
        // get size of the grid
        int getSize(); 

        // get the cell in row (row) and column (col) of grid to a pointer Title*
        Title* getCell(int row, int col); 

        // get the color of the cell in row (row) and column (col)
        bool getColor(int row, int col); 

        // get the positionStart of cell in row (row) and column (col)
        SDL_Point getCoor(int row, int col); 

        // get the gridRect of grid
        SDL_Rect* getGridRect(); 

        // find the position (row, col) of the cell that the mouse is pointing
        std::pair<int, int> identifyCell(); 

        // check if the mouse click action is correct (1) or not (0)
        bool validAction(std::pair<int, int> coorCell); 

        // for debugging 
        void debug(); 

    private:
        int sizeGrid; // size of the grid (= 5 if the grid is 5x5)
        int numberBlack; // number of black-cell
        Uint64 timeLimit; // time limit
        SDL_Rect gridRect; // an object stores information about the frame surrounding the grid
        std::vector<bool>currentGridColor; // a dynamic array store the color of all the cell of grid (0-white and 1-black)
        std::vector<std::vector<Title>>currentGrid; // a 2D dynamic array, each element is a Title object stores information about a cell
};

