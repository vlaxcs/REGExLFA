cmake_minimum_required(VERSION 3.30)
project(REGExLFA)

set(CMAKE_CXX_STANDARD 20)

add_executable(REGExLFA main.cpp
                src/PostfixConverter.cpp
                src/Tester.cpp)

include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_link_libraries(REGExLFA PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(REGExLFA PUBLIC include)
