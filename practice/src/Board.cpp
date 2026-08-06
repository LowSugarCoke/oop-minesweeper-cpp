#include "Board.h"
#include <random>
#include <stdexcept>
#include <algorithm>

// ==========================================
// 步驟 4: 實作 Board 建構子
// 初始化高、寬、地雷數，並建立一個 2D grid vector 容納每一個 Cell
// ==========================================
Board::Board(int width, int height, int minesCount) {
    // 1. 驗證 width, height, minesCount 參數：
    //    - 寬 (width) 與高 (height) 必須大於 0。
    //    - 地雷數 (minesCount) 必須大於 0。
    //    - 地雷數不能大於或等於總格子數 (width * height)。
    //    - 如果任何一個條件不符合，拋出 std::invalid_argument("例外原因說明...")。
    //
    // 2. 將屬性 (this->width, this->height, this->minesCount) 設定為傳入的值。
    //    - 將 isGameOver 設為 false。
    //    - 將 isGameWon 設為 false。
    //    - 將 isInitialized 設為 false (因為第一次點擊前還不隨機擺放地雷)。
    //
    // 3. 重塑 grid 的大小，並為其填充 Cell。
    //    - grid 的型態是 std::vector<std::vector<Cell>>。
    //    - grid[y][x] 代表位於 (x, y) 座標的格子，建構 Cell 時請記得將 x, y 傳入。
}

int Board::getWidth() const {
    return 0;
}

int Board::getHeight() const {
    return 0;
}

int Board::getMinesCount() const {
    return 0;
}

bool Board::getIsGameOver() const {
    return false;
}

bool Board::getIsGameWon() const {
    return false;
}

// 取得 (x, y) 的 Cell。若越界拋出 std::out_of_range 例外。
const Cell& Board::getCell(int x, int y) const {
    // 1. 檢查 x, y 座標是否越界 (x < 0 || x >= width || y < 0 || y >= height)。
    // 2. 如果越界，拋出 std::out_of_range("...") 例外。
    // 3. 如果合法，回傳 grid[y][x] 的常數參照。
    static Cell dummy;
    return dummy;
}

Cell& Board::getCell(int x, int y) {
    // 1. 檢查 x, y 座標是否越界 (x < 0 || x >= width || y < 0 || y >= height)。
    // 2. 如果越界，拋出 std::out_of_range("...") 例外。
    // 3. 如果合法，回傳 grid[y][x] 的參照。
    static Cell dummy;
    return dummy;
}

// ==========================================
// 步驟 5: 實作地雷隨機擺放邏輯
// 這是在第一次點擊 (reveal) 之後執行的私有方法，確保第一步不踩雷。
// ==========================================
void Board::placeMines(int startX, int startY) {
    // 1. 建立一個包含所有格子座標 (x, y) 的一維清單，排除起點座標 (startX, startY)
    //    (可選：也排除起點周圍的 8 個鄰近格子，這樣第一步可以點出一大片空地，體驗更好)。
    //
    // 2. 對清單進行隨機洗牌 (Random Shuffle)。
    //    - 提示：可以使用 std::random_device 與 std::mt19937 作為亂數引擎，配合 std::shuffle。
    //
    // 3. 取出隨機洗牌後的前 minesCount 個座標，將其設定為地雷。
    //    - 對每個選中的座標，呼叫其對應 Cell 的 setIsMine(true)。
}

// ==========================================
// 步驟 6: 計算每個非地雷格子的鄰近地雷數
// 也是在擺放完地雷後執行的私有方法。
// ==========================================
void Board::calculateNeighborMines() {
    // 1. 遍歷 grid 中的每一個格子。
    //
    // 2. 若該格子是地雷 (getIsMine() == true)，則跳過不計算。
    //
    // 3. 若格子不是地雷，遍歷其周圍的 8 個鄰近座標 (從 x-1 到 x+1，y-1 到 y+1)。
    //    - 排除越界的座標，以及格子本身。
    //    - 如果鄰近格子是地雷 (getIsMine() == true)，則計數加 1。
    //
    // 4. 計算結束後，將此鄰近地雷計數設定給該格子 (setNeighborMinesCount)。
}

// ==========================================
// 步驟 7: 實作翻開 (Reveal) 格子的邏輯
// ==========================================
void Board::reveal(int x, int y) {
    // 1. 檢查遊戲是否已經結束 (isGameOver == true)，若是，則不進行任何動作，直接返回。
    //
    // 2. 檢查座標是否越界，越界則拋出 std::out_of_range("...") 例外。
    //
    // 3. 取得該點 Cell。
    //    - 如果該格子已經被翻開 (getIsRevealed() == true) 或已經插旗標記 (getIsFlagged() == true)，則直接返回。
    //
    // 4. 檢查 Board 否已被安全初始化。
    //    - 如果 isInitialized == false，代表這是玩家的「第一點」：
    //      - 呼叫 placeMines(x, y) 隨機擺放地雷並確保 (x, y) 安全。
    //      - 呼叫 calculateNeighborMines() 計算地雷鄰近數。
    //      - 將 isInitialized 設為 true。
    //
    // 5. 翻開格子：
    //    - 呼叫該格子的 setIsRevealed(true)。
    //
    // 6. 如果該格子是地雷：
    //    - 遊戲失敗！將 isGameOver 設為 true，並保持 isGameWon 為 false。
    //    - (可選) 遍歷整個 grid，將所有的地雷都翻開 (setIsRevealed(true))，讓玩家死個明白！
    //    - 直接返回。
    //
    // 7. 如果該格子不是地雷：
    //    - 檢查該格子鄰近地雷數 (getNeighborMinesCount())。
    //    - 如果為 0，代表此處是安全的一大片空地，必須遞迴(或使用佇列/堆疊)將周圍鄰近 8 個格子的「未翻開且未插旗」格子自動翻開 (自動呼叫 reveal)。
    //
    // 8. 呼叫 checkWinCondition() 檢查玩家是否已經達成了獲勝條件。
}

// ==========================================
// 步驟 8: 實作插旗與取消插旗 (Toggle Flag) 邏輯
// ==========================================
void Board::toggleFlag(int x, int y) {
    // 1. 檢查遊戲是否已經結束 (isGameOver == true)，如果是，直接返回。
    //
    // 2. 檢查座標是否越界，越界則拋出 std::out_of_range("...") 例外。
    //
    // 3. 取得格子 Cell。
    //    - 如果該格子已經被翻開，則不能插旗，直接返回。
    //
    // 4. 反轉插旗狀態：
    //    - 呼叫 setIsFlagged(!getIsFlagged())。
}

// ==========================================
// 步驟 9: 實作檢查獲勝條件邏輯
// ==========================================
void Board::checkWinCondition() {
    // 1. 遍歷 grid 中的每一個格子。
    //
    // 2. 檢查獲勝條件：是否所有「非地雷」的格子都被翻開了？
    //    - 如果有任何一個不是地雷的格子「未被翻開」，則不滿足獲勝條件，直接返回。
    //
    // 3. 如果滿足條件，玩家獲勝！
    //    - 將 isGameWon 設為 true。
    //    - 將 isGameOver 設為 true。
}
