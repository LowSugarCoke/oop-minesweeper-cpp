#include "Board.h"
#include <random>
#include <stdexcept>
#include <algorithm>

Board::Board(int width, int height, int minesCount) {
    if (width <= 0 || height <= 0 || minesCount <= 0) {
        throw std::invalid_argument("Width, height, and mines count must be greater than 0");
    }
    if (minesCount >= width * height) {
        throw std::invalid_argument("Mines count cannot be greater than or equal to total cell count");
    }

    this->width = width;
    this->height = height;
    this->minesCount = minesCount;
    this->isGameOver = false;
    this->isGameWon = false;
    this->isInitialized = false;

    grid.resize(height);
    for (int y = 0; y < height; ++y) {
        grid[y].reserve(width);
        for (int x = 0; x < width; ++x) {
            grid[y].push_back(Cell(x, y));
        }
    }
}

int Board::getWidth() const {
    return width;
}

int Board::getHeight() const {
    return height;
}

int Board::getMinesCount() const {
    return minesCount;
}

bool Board::getIsGameOver() const {
    return isGameOver;
}

bool Board::getIsGameWon() const {
    return isGameWon;
}

const Cell& Board::getCell(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::out_of_range("Coordinates out of bounds");
    }
    return grid[y][x];
}

Cell& Board::getCell(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::out_of_range("Coordinates out of bounds");
    }
    return grid[y][x];
}

void Board::placeMines(int startX, int startY) {
    std::vector<std::pair<int, int>> potentialCoords;
    
    // Check if we have enough space to exclude the first click's 3x3 surrounding area.
    // Standard rule: try to exclude the starting cell and its 8 neighbors.
    bool excludeNeighbors = (width * height - 9 >= minesCount);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (x == startX && y == startY) {
                continue; // Always exclude start point
            }
            if (excludeNeighbors && std::abs(x - startX) <= 1 && std::abs(y - startY) <= 1) {
                continue; // Exclude 8 neighbors if enough space exists
            }
            potentialCoords.push_back({x, y});
        }
    }

    // Fallback: if we chose to exclude neighbors but didn't have enough squares, try with only excluding starting point
    if (potentialCoords.size() < static_cast<size_t>(minesCount)) {
        potentialCoords.clear();
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (x == startX && y == startY) {
                    continue;
                }
                potentialCoords.push_back({x, y});
            }
        }
    }

    // Shuffle the coordinates
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(potentialCoords.begin(), potentialCoords.end(), g);

    // Place mines in the first 'minesCount' coordinates
    for (int i = 0; i < minesCount && i < static_cast<int>(potentialCoords.size()); ++i) {
        int mx = potentialCoords[i].first;
        int my = potentialCoords[i].second;
        grid[my][mx].setIsMine(true);
    }
}

void Board::calculateNeighborMines() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (grid[y][x].getIsMine()) {
                continue;
            }

            int count = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        if (grid[ny][nx].getIsMine()) {
                            count++;
                        }
                    }
                }
            }
            grid[y][x].setNeighborMinesCount(count);
        }
    }
}

void Board::reveal(int x, int y) {
    if (isGameOver) {
        return;
    }
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::out_of_range("Coordinates out of bounds");
    }

    Cell& cell = grid[y][x];
    if (cell.getIsRevealed() || cell.getIsFlagged()) {
        return;
    }

    if (!isInitialized) {
        placeMines(x, y);
        calculateNeighborMines();
        isInitialized = true;
    }

    // Re-fetch reference in case vectors moved, though size is static after constructor
    Cell& c = grid[y][x];
    c.setIsRevealed(true);

    if (c.getIsMine()) {
        isGameOver = true;
        isGameWon = false;
        // Optionally reveal all other mines
        for (int h = 0; h < height; ++h) {
            for (int w = 0; w < width; ++w) {
                if (grid[h][w].getIsMine()) {
                    grid[h][w].setIsRevealed(true);
                }
            }
        }
        return;
    }

    if (c.getNeighborMinesCount() == 0) {
        // Cascade reveal neighbors using flood fill DFS/BFS
        std::vector<std::pair<int, int>> queue;
        queue.push_back({x, y});

        size_t index = 0;
        while (index < queue.size()) {
            auto curr = queue[index++];
            int cx = curr.first;
            int cy = curr.second;

            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = cx + dx;
                    int ny = cy + dy;

                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        Cell& neighbor = grid[ny][nx];
                        if (!neighbor.getIsRevealed() && !neighbor.getIsFlagged() && !neighbor.getIsMine()) {
                            neighbor.setIsRevealed(true);
                            if (neighbor.getNeighborMinesCount() == 0) {
                                queue.push_back({nx, ny});
                            }
                        }
                    }
                }
            }
        }
    }

    checkWinCondition();
}

void Board::toggleFlag(int x, int y) {
    if (isGameOver) {
        return;
    }
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::out_of_range("Coordinates out of bounds");
    }

    Cell& cell = grid[y][x];
    if (cell.getIsRevealed()) {
        return;
    }

    cell.setIsFlagged(!cell.getIsFlagged());
}

void Board::checkWinCondition() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const Cell& cell = grid[y][x];
            if (!cell.getIsMine() && !cell.getIsRevealed()) {
                // If any non-mine cell is not revealed, game is not won yet
                return;
            }
        }
    }

    isGameWon = true;
    isGameOver = true;
}
