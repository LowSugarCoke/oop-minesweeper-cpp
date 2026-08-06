#include "Cell.h"

// ==========================================
// 步驟 1: 實作預設建構子
// 將座標 (x, y) 設為 0
// 所有布林狀態 (isMine, isRevealed, isFlagged) 設為 false
// 鄰近地雷數 (neighborMinesCount) 設為 0
// ==========================================
Cell::Cell() {
    // 1. 初始化成員變數：
    //    x = 0;
    //    y = 0;
    //    isMine = false;
    //    isRevealed = false;
    //    isFlagged = false;
    //    neighborMinesCount = 0;
}

// ==========================================
// 步驟 2: 實作帶座標的建構子
// 將 x, y 設為傳入的參數值
// 其餘狀態 (isMine, isRevealed, isFlagged, neighborMinesCount) 與預設建構子相同
// ==========================================
Cell::Cell(int x, int y) {
    // 1. 初始化成員變數：
    //    this->x = x;
    //    this->y = y;
    //    isMine = false;
    //    isRevealed = false;
    //    isFlagged = false;
    //    neighborMinesCount = 0;
}

// ==========================================
// 步驟 3: 實作各個 Getter 與 Setter
// 讓外界或 Board 類別可以讀寫格子的屬性
// ==========================================

int Cell::getX() const {
    // 回傳格子 x 座標
    return 0;
}

int Cell::getY() const {
    // 回傳格子 y 座標
    return 0;
}

bool Cell::getIsMine() const {
    // 回傳該格子是否為地雷
    return false;
}

void Cell::setIsMine(bool val) {
    // 設定該格子是否為地雷
}

bool Cell::getIsRevealed() const {
    // 回傳該格子是否已被翻開
    return false;
}

void Cell::setIsRevealed(bool val) {
    // 設定該格子已被翻開
}

bool Cell::getIsFlagged() const {
    // 回傳該格子是否已被插旗標記
    return false;
}

void Cell::setIsFlagged(bool val) {
    // 設定該格子已被插旗標記
}

int Cell::getNeighborMinesCount() const {
    // 回傳該格子鄰近 8 格的地雷數
    return 0;
}

void Cell::setNeighborMinesCount(int val) {
    // 設定該格子鄰近 8 格的地雷數
}
