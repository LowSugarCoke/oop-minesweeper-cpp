#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <fstream>
#include "httplib.h"
#include "json.hpp"
#include "Board.h"

using json = nlohmann::json;

static std::unique_ptr<Board> g_board = nullptr;
static std::string g_init_error = "";
static int g_current_width = 9;
static int g_current_height = 9;
static int g_current_mines = 10;

std::string resetGameBoard(int width, int height, int mines) {
    try {
        g_board = std::make_unique<Board>(width, height, mines);
        g_current_width = width;
        g_current_height = height;
        g_current_mines = mines;
        g_init_error = "";
        return "";
    } catch (const std::exception& e) {
        g_board = nullptr;
        g_init_error = e.what();
        return g_init_error;
    } catch (...) {
        g_board = nullptr;
        g_init_error = "Unknown error during Board initialization";
        return g_init_error;
    }
}

Board* getBoard() {
    if (!g_board && g_init_error.empty()) {
        resetGameBoard(g_current_width, g_current_height, g_current_mines);
    }
    if (!g_board) {
        throw std::runtime_error(g_init_error.empty() ? "Game board is not initialized" : g_init_error);
    }
    return g_board.get();
}

json serializeBoard(Board* board) {
    json j;
    j["width"] = board->getWidth();
    j["height"] = board->getHeight();
    j["minesCount"] = board->getMinesCount();
    j["isGameOver"] = board->getIsGameOver();
    j["isGameWon"] = board->getIsGameWon();

    json gridJson = json::array();
    for (int y = 0; y < board->getHeight(); ++y) {
        json rowJson = json::array();
        for (int x = 0; x < board->getWidth(); ++x) {
            const Cell& cell = board->getCell(x, y);
            json cellJson;
            cellJson["x"] = cell.getX();
            cellJson["y"] = cell.getY();
            cellJson["isMine"] = cell.getIsMine();
            cellJson["isRevealed"] = cell.getIsRevealed();
            cellJson["isFlagged"] = cell.getIsFlagged();
            cellJson["neighborMines"] = cell.getNeighborMinesCount();
            rowJson.push_back(cellJson);
        }
        gridJson.push_back(rowJson);
    }
    j["grid"] = gridJson;
    return j;
}

std::string runTestsAndGetOutput() {
    FILE* pipe = popen("./tests", "r");
    if (!pipe) {
        return "{\"error\": \"Failed to run tests binary\"}";
    }
    char buffer[256];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

// Find a file in local directory or parent directory
std::string findStaticFile(const std::string& path) {
    // Try current path
    std::ifstream f(path);
    if (f.good()) return path;
    
    // Try parent path
    std::ifstream f2("../" + path);
    if (f2.good()) return "../" + path;

    // Try custom folder
    std::ifstream f3("public/" + path);
    if (f3.good()) return "public/" + path;

    std::ifstream f4("../public/" + path);
    if (f4.good()) return "../public/" + path;

    return "";
}

int main() {
    httplib::Server svr;

    // Route: GET /api/state
    svr.Get("/api/state", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            Board* board = getBoard();
            res.set_content(serializeBoard(board).dump(), "application/json");
        } catch (const std::exception& e) {
            json errorJson;
            errorJson["error"] = e.what();
            res.status = 500;
            res.set_content(errorJson.dump(), "application/json");
        }
    });

    // Route: POST /api/action
    svr.Post("/api/action", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            auto bodyJson = json::parse(req.body);
            std::string action = bodyJson.at("action").get<std::string>();
            auto params = bodyJson.value("params", json::object());

            if (action == "reset") {
                int width = params.value("width", 9);
                int height = params.value("height", 9);
                int mines = params.value("mines", 10);
                std::string err = resetGameBoard(width, height, mines);
                if (!err.empty()) {
                    json errorJson;
                    errorJson["error"] = err;
                    res.status = 400;
                    res.set_content(errorJson.dump(), "application/json");
                    return;
                }
            } else {
                Board* board = getBoard();
                if (action == "reveal") {
                    int x = params.at("x").get<int>();
                    int y = params.at("y").get<int>();
                    board->reveal(x, y);
                } else if (action == "flag") {
                    int x = params.at("x").get<int>();
                    int y = params.at("y").get<int>();
                    board->toggleFlag(x, y);
                } else {
                    json errorJson;
                    errorJson["error"] = "Unknown action: " + action;
                    res.status = 400;
                    res.set_content(errorJson.dump(), "application/json");
                    return;
                }
            }

            Board* board = getBoard();
            res.set_content(serializeBoard(board).dump(), "application/json");
        } catch (const json::exception& e) {
            json errorJson;
            errorJson["error"] = std::string("JSON parsing error: ") + e.what();
            res.status = 400;
            res.set_content(errorJson.dump(), "application/json");
        } catch (const std::exception& e) {
            json errorJson;
            errorJson["error"] = e.what();
            res.status = 400;
            res.set_content(errorJson.dump(), "application/json");
        }
    });

    // Route: POST /api/tests/run
    svr.Post("/api/tests/run", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        std::string jsonStr = runTestsAndGetOutput();
        res.set_content(jsonStr, "application/json");
    });

    // Serve Static Files manually or via mount point
    // Since we want robust fallback, let's write a catch-all route for static files!
    svr.Get("/(.*)", [](const httplib::Request& req, httplib::Response& res) {
        std::string target = req.path == "/" ? "index.html" : req.path.substr(1);
        std::string actualPath = findStaticFile(target);
        
        if (actualPath.empty()) {
            res.status = 404;
            res.set_content("File Not Found: " + target, "text/plain");
            return;
        }

        std::ifstream in(actualPath, std::ios::binary);
        if (!in) {
            res.status = 404;
            res.set_content("File Not Found: " + target, "text/plain");
            return;
        }

        std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        
        std::string mime = "text/plain";
        if (target.size() >= 5 && target.substr(target.size() - 5) == ".html") mime = "text/html";
        else if (target.size() >= 3 && target.substr(target.size() - 3) == ".js") mime = "application/javascript";
        else if (target.size() >= 4 && target.substr(target.size() - 4) == ".css") mime = "text/css";
        else if (target.size() >= 4 && target.substr(target.size() - 4) == ".png") mime = "image/png";

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(content, mime);
    });

    std::cout << "Server starting on http://localhost:8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}
