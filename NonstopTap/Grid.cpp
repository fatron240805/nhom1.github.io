#include <SDL2/SDL.h>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iostream>
#include <assert.h>

#include "Grid.h"
#include "Title.h"

Grid::Grid()
{
    sizeGrid = 0;
    numberBlack = 0;
    timeLimit = 0;
}

Grid::~Grid()
{
    currentGridColor.clear();
    currentGrid.clear();
}

Grid::Grid(int __sizeGrid, Uint64 __timeLimit, int __numBlack, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    sizeGrid = __sizeGrid;
    timeLimit = __timeLimit;
    numberBlack = __numBlack;

    currentGridColor.assign(sizeGrid * sizeGrid - numberBlack, 0);
    currentGridColor.insert(currentGridColor.end(), numberBlack, 1);
    currentGrid.assign(sizeGrid, std::vector<Title>(sizeGrid, Title()));

    int lengthGrid = sizeGrid * SIZE_CELL + (sizeGrid - 1) * DISTANCE_CELL;
    int startX = (SCREEN_WIDTH - lengthGrid) / 2, startY = (SCREEN_HEIGHT - lengthGrid) / 2;
    gridRect = {startX - DISTANCE_CELL, startY - DISTANCE_CELL + 60, lengthGrid + DISTANCE_CELL * 2, lengthGrid + DISTANCE_CELL * 2};

    generateBlackCell();
}

void Grid::generateBlackCell()
{
    std::random_shuffle(currentGridColor.begin(), currentGridColor.end());

    for (int i = 0; i < sizeGrid * sizeGrid; i++)
    {
        int row = i / sizeGrid;
        int col = i % sizeGrid;
        currentGrid[row][col] = Title(currentGridColor[i], gridRect.x + DISTANCE_CELL + (SIZE_CELL + DISTANCE_CELL) * col, gridRect.y + DISTANCE_CELL + (SIZE_CELL + DISTANCE_CELL) * row);
    }
}

int Grid::getSize()
{
    return sizeGrid;
}

Title* Grid::getCell(int row, int col)
{
    return &currentGrid[row][col];
}

bool Grid::getColor(int row, int col)
{
    return currentGrid[row][col].getColor();
}

SDL_Rect* Grid::getGridRect()
{
    return &gridRect;
}

void Grid::onlyChangeColor(int row, int col)
{
    currentGrid[row][col].flipColor();
}

void Grid::changeColor(int row, int col)
{
    bool color = currentGrid[row][col].getColor();
    currentGrid[row][col].flipColor();
    currentGridColor[row * sizeGrid + col].flip();

    if (color == BLACK_CELL)
        assert(count(currentGridColor.begin(), currentGridColor.end(), true) == numberBlack - 1);

    int nextRandomIndex;
    do
    {
        nextRandomIndex = rand() % (sizeGrid * sizeGrid);
    } while (nextRandomIndex == row * sizeGrid + col || currentGridColor[nextRandomIndex] == color);

    assert(nextRandomIndex >= 0 && nextRandomIndex < sizeGrid * sizeGrid);
    
    int nextRow = nextRandomIndex / sizeGrid;
    int nextCol = nextRandomIndex % sizeGrid;
    currentGrid[nextRow][nextCol].flipColor();
    currentGridColor[nextRandomIndex].flip();
}

SDL_Point Grid::getCoor(int row, int col)
{
    return currentGrid[row][col].getPosition();
}

//unoptimized
std::pair<int, int> Grid::identifyCell()
{
    int x, y;
    SDL_GetMouseState(&x, &y);

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

    return std::make_pair(-1, -1);
}

bool Grid::validAction(std::pair<int, int> coorCell)
{
    return currentGrid[coorCell.first][coorCell.second].getColor();
}

void Grid::debug()
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
