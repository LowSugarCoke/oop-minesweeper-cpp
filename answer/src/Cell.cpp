#include "Cell.h"

Cell::Cell() {
    x = 0;
    y = 0;
    isMine = false;
    isRevealed = false;
    isFlagged = false;
    neighborMinesCount = 0;
}

Cell::Cell(int x, int y) {
    this->x = x;
    this->y = y;
    isMine = false;
    isRevealed = false;
    isFlagged = false;
    neighborMinesCount = 0;
}

int Cell::getX() const {
    return x;
}

int Cell::getY() const {
    return y;
}

bool Cell::getIsMine() const {
    return isMine;
}

void Cell::setIsMine(bool val) {
    isMine = val;
}

bool Cell::getIsRevealed() const {
    return isRevealed;
}

void Cell::setIsRevealed(bool val) {
    isRevealed = val;
}

bool Cell::getIsFlagged() const {
    return isFlagged;
}

void Cell::setIsFlagged(bool val) {
    isFlagged = val;
}

int Cell::getNeighborMinesCount() const {
    return neighborMinesCount;
}

void Cell::setNeighborMinesCount(int val) {
    neighborMinesCount = val;
}
