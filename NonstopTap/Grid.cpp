#include <SDL2/SDL.h>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iostream>
#include <assert.h>

#include "Grid.h"
#include "Title.h"

Grid::Grid() // initialize 
{
    sizeGrid = 0;
    numberBlack = 0;
    timeLimit = 0;
}

Grid::~Grid() // free the memory
{
    currentGridColor.clear();
    currentGrid.clear();
}

Grid::Grid(int __sizeGrid, Uint64 __timeLimit, int __numBlack, int SCREEN_WIDTH, int SCREEN_HEIGHT) // initialize the grid with it's properties
{
    sizeGrid = __sizeGrid;
    timeLimit = __timeLimit;
    numberBlack = __numBlack;

    currentGridColor.assign(sizeGrid * sizeGrid - numberBlack, 0); // a dynamic array with all element is 0
    currentGridColor.insert(currentGridColor.end(), numberBlack, 1); // add (numberBlack) element(s) 1 
    currentGrid.assign(sizeGrid, std::vector<Title>(sizeGrid, Title())); // a 2D dynamic array (sizeGrid x sizeGrid) with all elements is Tille's object

    int lengthGrid = sizeGrid * SIZE_CELL + (sizeGrid - 1) * DISTANCE_CELL; // length of the square grid
    int startX = (SCREEN_WIDTH - lengthGrid) / 2, startY = (SCREEN_HEIGHT - lengthGrid) / 2; // positionStart (the point in the upper left) of the grid
    gridRect = {startX - DISTANCE_CELL, startY - DISTANCE_CELL + 60, lengthGrid + DISTANCE_CELL * 2, lengthGrid + DISTANCE_CELL * 2}; // initialize the gridRect

    generateBlackCell();
}

void Grid::generateBlackCell()// randomly generate black cells
{
    std::random_shuffle(currentGridColor.begin(), currentGridColor.end()); // random the element 0 (while) and 1 (black) in array currentGridColor

    // re-setup the property (color) for all the cells in grid
    for (int i = 0; i < sizeGrid * sizeGrid; i++)
    {
        int row = i / sizeGrid;
        int col = i % sizeGrid;
        currentGrid[row][col] = Title(currentGridColor[i], gridRect.x + DISTANCE_CELL + (SIZE_CELL + DISTANCE_CELL) * col, gridRect.y + DISTANCE_CELL + (SIZE_CELL + DISTANCE_CELL) * row);
    }
}

void Grid::onlyChangeColor(int row, int col) // change color of cell only 
{
    currentGrid[row][col].flipColor();
}

void Grid::changeColor(int row, int col) // change the color and state of cell
{
    // save the color of cell to variable color then flip the color of that cell
    bool color = currentGrid[row][col].getColor();
    currentGrid[row][col].flipColor();
    currentGridColor[row * sizeGrid + col].flip();

    // check if the number of black-cell now is equal to numberBlack - 1
    if (color == BLACK_CELL)
        assert(count(currentGridColor.begin(), currentGridColor.end(), true) == numberBlack - 1);

    // choose a random cell, sastify that it is not the cell (row, col) and do not have the same color as the original color of this cell (row, col)
    int nextRandomIndex;
    do
    {
        nextRandomIndex = rand() % (sizeGrid * sizeGrid);
    } while (nextRandomIndex == row * sizeGrid + col || currentGridColor[nextRandomIndex] == color);

    // check if the nextRandomIndex is legal
    assert(nextRandomIndex >= 0 && nextRandomIndex < sizeGrid * sizeGrid);
    
    // change color of the cell which is randomly choose 
    int nextRow = nextRandomIndex / sizeGrid;
    int nextCol = nextRandomIndex % sizeGrid;
    currentGrid[nextRow][nextCol].flipColor();
    currentGridColor[nextRandomIndex].flip();
}

int Grid::getSize() // get size of grid (4x4, 5x5, ...)
{
    return sizeGrid;
}

Title* Grid::getCell(int row, int col) // get the cell in row (row) and column (col) of grid
{
    return &currentGrid[row][col];
}

bool Grid::getColor(int row, int col) // get the color of the cell in row (row) and column (col)
{
    return currentGrid[row][col].getColor();
}

SDL_Point Grid::getCoor(int row, int col) // get the positionStart of cell in row (row) and column (col)
{
    return currentGrid[row][col].getPosition();
}

SDL_Rect* Grid::getGridRect() // get the gridRect of grid
{
    return &gridRect;
}

std::pair<int, int> Grid::identifyCell() // find the position (row, col) of the cell that the mouse is pointing
{
    int x, y;
    SDL_GetMouseState(&x, &y); // get the position of the mouse

    // find the position of cell that the mouse is pointing
    for (int row = 0; row < sizeGrid; row++)
    {
        for (int col = 0; col < sizeGrid; col++)
        {
            if (currentGrid[row][col].insideCell(x, y))
            {
                return std::make_pair(row, col);
            }
        }
    }

    return std::make_pair(-1, -1); // if there is no cell sastify, return (-1, -1)
    

    // int lengthGrid = sizeGrid * SIZE_CELL + (sizeGrid - 1) * DISTANCE_CELL; // length of the square grid
    // int startX = (SCREEN_WIDTH - lengthGrid) / 2, startY = (SCREEN_HEIGHT - lengthGrid) / 2; // positionStart (the point in the upper left) of the grid
    // int row, col;

    // bool flag = 1;
    // if (x < startX || (SCREEN_WIDTH - x) < startX || y < startY + 60 || (SCREEN_HEIGHT - y) < (startY - 60))
    // {
    //     flag = 0;
    // }
    // else 
    // {
    //     int row = (y - startY - 60) / (SIZE_CELL + DISTANCE_CELL);
    //     int col = (x - startX) / (SIZE_CELL + DISTANCE_CELL);
        
    //     if (SIZE_CELL < ((y - startY - 60) % (SIZE_CELL + DISTANCE_CELL)) && ((y - startY - 60) % (SIZE_CELL + DISTANCE_CELL)) < SIZE_CELL + DISTANCE_CELL)
    //     {
    //         flag = 0;
    //     }
    //     else if (SIZE_CELL < ((x - startX) % (SIZE_CELL + DISTANCE_CELL)) && ((x - startX) % (SIZE_CELL + DISTANCE_CELL)) < SIZE_CELL + DISTANCE_CELL)
    //     {
    //         flag = 0;
    //     }
    //     else
    //     {
    //         flag = 1;
    //     }
    // }

    // if {flag == 0}
    // {
    //     return std::make_pair(-1, -1);
    // }
    // else 
    // {
    //     return std::make_pair(row, col);
    // }
    
}

bool Grid::validAction(std::pair<int, int> coorCell) // check if the mouse click action is correct or not
{
    return currentGrid[coorCell.first][coorCell.second].getColor();
}

void Grid::debug() // for debugging
{
    std::cout << "Current grid" << std::endl;
    for (int row = 0; row < sizeGrid; row++)
    {
        for (int col = 0; col < sizeGrid; col++)
        {
            std::cout << currentGrid[row][col].getColor() << ' ';
        }
        std::cout << std::endl;
    }
}
