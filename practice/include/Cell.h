#ifndef CELL_H
#define CELL_H

class Cell {
private:
    int x;
    int y;
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int neighborMinesCount;

public:
    Cell();
    Cell(int x, int y);

    int getX() const;
    int getY() const;

    bool getIsMine() const;
    void setIsMine(bool val);

    bool getIsRevealed() const;
    void setIsRevealed(bool val);

    bool getIsFlagged() const;
    void setIsFlagged(bool val);

    int getNeighborMinesCount() const;
    void setNeighborMinesCount(int val);
};

#endif // CELL_H
