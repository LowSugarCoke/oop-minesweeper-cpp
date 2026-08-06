#include "TestFramework.h"
#include "Cell.h"
#include "Board.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

TEST_CASE("Cell - 預設建構子與 Getter/Setter 測試") {
    Cell cell;
    REQUIRE_EQ(cell.getX(), 0);
    REQUIRE_EQ(cell.getY(), 0);
    REQUIRE(!cell.getIsMine());
    REQUIRE(!cell.getIsRevealed());
    REQUIRE(!cell.getIsFlagged());
    REQUIRE_EQ(cell.getNeighborMinesCount(), 0);

    cell.setIsMine(true);
    cell.setIsRevealed(true);
    cell.setIsFlagged(true);
    cell.setNeighborMinesCount(3);

    REQUIRE(cell.getIsMine());
    REQUIRE(cell.getIsRevealed());
    REQUIRE(cell.getIsFlagged());
    REQUIRE_EQ(cell.getNeighborMinesCount(), 3);
}

TEST_CASE("Cell - 帶座標的建構子測試") {
    Cell cell(5, 8);
    REQUIRE_EQ(cell.getX(), 5);
    REQUIRE_EQ(cell.getY(), 8);
    REQUIRE(!cell.getIsMine());
}

TEST_CASE("Board - 建構子參數驗證測試") {
    // 正常建構
    Board b(10, 10, 10);
    REQUIRE_EQ(b.getWidth(), 10);
    REQUIRE_EQ(b.getHeight(), 10);
    REQUIRE_EQ(b.getMinesCount(), 10);
    REQUIRE(!b.getIsGameOver());
    REQUIRE(!b.getIsGameWon());

    // 異常輸入驗證 - 寬高為零或負數
    bool caught1 = false;
    try {
        Board invalid(0, 5, 2);
    } catch (const std::invalid_argument&) {
        caught1 = true;
    }
    REQUIRE(caught1);

    // 異常輸入驗證 - 地雷數過多
    bool caught2 = false;
    try {
        Board invalid(5, 5, 25); // 地雷數 >= 格子總數
    } catch (const std::invalid_argument&) {
        caught2 = true;
    }
    REQUIRE(caught2);
}

TEST_CASE("Board - 邊界越界驗證測試") {
    Board b(5, 5, 3);
    
    bool caughtReveal = false;
    try {
        b.reveal(-1, 0);
    } catch (const std::out_of_range&) {
        caughtReveal = true;
    }
    REQUIRE(caughtReveal);

    caughtReveal = false;
    try {
        b.reveal(5, 2);
    } catch (const std::out_of_range&) {
        caughtReveal = true;
    }
    REQUIRE(caughtReveal);

    bool caughtFlag = false;
    try {
        b.toggleFlag(0, -1);
    } catch (const std::out_of_range&) {
        caughtFlag = true;
    }
    REQUIRE(caughtFlag);
}

TEST_CASE("Board - 第一次點擊安全初始化測試") {
    Board b(5, 5, 5);
    
    // 初始化前，所有格子都不是地雷 (因為 placeMines 還沒被執行)
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            REQUIRE(!b.getCell(x, y).getIsMine());
        }
    }

    // 第一點點擊 (2, 2) 觸發初始化
    b.reveal(2, 2);

    // 點擊點絕不能是地雷
    REQUIRE(!b.getCell(2, 2).getIsMine());
    REQUIRE(b.getCell(2, 2).getIsRevealed());

    // 地雷應已被成功擺放 5 顆
    int actualMines = 0;
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (b.getCell(x, y).getIsMine()) {
                actualMines++;
            }
        }
    }
    REQUIRE_EQ(actualMines, 5);
}

TEST_CASE("Board - 遞迴/連鎖翻開空地測試") {
    // 建立 5x5 棋盤，擺放 1 顆地雷。
    // 依據安全第一步規則，當寬高為 5x5 且雷數為 1 時，寬高-9 (16) >= 雷數 (1)，
    // 系統會 100% 保證起點 (2, 2) 及其周圍 8 個鄰近格子都不會有雷（雷一定會擺在最外圈）。
    // 這樣可以 100% 確保 (2, 2) 的鄰近地雷數為 0，且點擊後會連鎖翻開周圍的所有格子！
    Board b(5, 5, 1);
    
    // 第一次點擊正中心 (2, 2)
    b.reveal(2, 2);

    // 正中心與其周圍 8 個格子都應該被翻開
    REQUIRE(b.getCell(2, 2).getIsRevealed());
    REQUIRE(b.getCell(1, 1).getIsRevealed());
    REQUIRE(b.getCell(1, 2).getIsRevealed());
    REQUIRE(b.getCell(1, 3).getIsRevealed());
    REQUIRE(b.getCell(2, 1).getIsRevealed());
    REQUIRE(b.getCell(2, 3).getIsRevealed());
    REQUIRE(b.getCell(3, 1).getIsRevealed());
    REQUIRE(b.getCell(3, 2).getIsRevealed());
    REQUIRE(b.getCell(3, 3).getIsRevealed());
}

TEST_CASE("Board - 踩雷輸掉與翻開非雷獲勝測試") {
    // 1. 測試踩雷
    {
        Board b(3, 3, 1);
        b.reveal(1, 1); // 第一點安全初始化

        // 找到隨機擺放的那顆地雷座標
        int mineX = -1, mineY = -1;
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                if (b.getCell(x, y).getIsMine()) {
                    mineX = x;
                    mineY = y;
                    break;
                }
            }
        }

        REQUIRE(mineX != -1);
        b.reveal(mineX, mineY); // 踩雷
        REQUIRE(b.getIsGameOver());
        REQUIRE(!b.getIsGameWon());
    }

    // 2. 測試獲勝：3x3 棋盤，1 顆雷，翻開所有 8 個非雷格子
    {
        Board b(3, 3, 1);
        b.reveal(0, 0); // 第一點安全初始化

        // 翻開所有不是地雷的格子
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                if (!b.getCell(x, y).getIsMine()) {
                    b.reveal(x, y);
                }
            }
        }

        REQUIRE(b.getIsGameOver());
        REQUIRE(b.getIsGameWon());
    }
}

// JSON 字串逸出輔助函式
std::string escapeJSON(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\b') out += "\\b";
        else if (c == '\f') out += "\\f";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else if (((unsigned char)c) < 32) {
            // ignore
        } else {
            out += c;
        }
    }
    return out;
}

int main() {
    const auto& testCases = TestRegistry::getInstance().getTestCases();
    int passed = 0;
    int failed = 0;

    std::stringstream testsJson;

    for (size_t i = 0; i < testCases.size(); ++i) {
        const auto& tc = testCases[i];
        std::string status = "passed";
        std::string message = "";
        try {
            tc.run();
            passed++;
        } catch (const TestAssertionException& e) {
            status = "failed";
            message = e.what();
            failed++;
        } catch (const std::exception& e) {
            status = "failed";
            message = std::string("Unexpected exception: ") + e.what();
            failed++;
        } catch (...) {
            status = "failed";
            message = "Unknown exception thrown";
            failed++;
        }

        testsJson << "    {\n";
        testsJson << "      \"name\": \"" << escapeJSON(tc.name) << "\",\n";
        testsJson << "      \"status\": \"" << status << "\"";
        if (status == "failed") {
            testsJson << ",\n      \"message\": \"" << escapeJSON(message) << "\"";
        }
        testsJson << "\n    }";
        if (i + 1 < testCases.size()) {
            testsJson << ",";
        }
        testsJson << "\n";
    }

    std::cout << "{\n";
    std::cout << "  \"passed\": " << passed << ",\n";
    std::cout << "  \"failed\": " << failed << ",\n";
    std::cout << "  \"tests\": [\n" << testsJson.str() << "  ]\n";
    std::cout << "}\n";

    return failed == 0 ? 0 : 1;
}
