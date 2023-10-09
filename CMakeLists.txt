cmake_minimum_required(VERSION 3.25.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chess LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Type")

add_executable(chess)
target_sources(chess PRIVATE src/main.cpp
                             src/BoardManager.cpp
                             src/Game.cpp
                             src/App.cpp)


target_include_directories(chess PRIVATE include)

target_link_libraries(chess)
