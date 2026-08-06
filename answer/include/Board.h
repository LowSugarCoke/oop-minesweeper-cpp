#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include "Cell.h"

class Board {
private:
    int width;
    int height;
    int minesCount;
    std::vector<std::vector<Cell>> grid;
    bool isGameOver;
    bool isGameWon;
    bool isInitialized;

    void placeMines(int startX, int startY);
    void calculateNeighborMines();

public:
    Board(int width, int height, int minesCount);

    int getWidth() const;
    int getHeight() const;
    int getMinesCount() const;

    bool getIsGameOver() const;
    bool getIsGameWon() const;

    const Cell& getCell(int x, int y) const;
    Cell& getCell(int x, int y);

    void reveal(int x, int y);
    void toggleFlag(int x, int y);
    void checkWinCondition();
};

#endif // BOARD_H
