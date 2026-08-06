cmake_minimum_required(VERSION 3.15)
project(MinesweeperPractice CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch cpp-httplib (header-only HTTP library)
FetchContent_Declare(httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.52.0)
FetchContent_MakeAvailable(httplib)

# Fetch nlohmann/json (header-only JSON library)
FetchContent_Declare(json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

include_directories(include)

# Test executable (synchronously executed by backend_adapter)
add_executable(tests
    src/Cell.cpp
    src/Board.cpp
    src/tests.cpp
)
target_link_libraries(tests PRIVATE nlohmann_json::nlohmann_json)

# Web server backend adapter executable
add_executable(backend_adapter
    src/Cell.cpp
    src/Board.cpp
    src/adapter.cpp
)
target_link_libraries(backend_adapter PRIVATE httplib::httplib nlohmann_json::nlohmann_json)
