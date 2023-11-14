cmake_minimum_required(VERSION 3.25.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chess LANGUAGES C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_BUILD_TYPE "Debug")

add_executable(chess)
target_sources(chess PRIVATE src/main.cpp
                             src/BoardManager.cpp
                             src/Game.cpp
                             src/App.cpp
                             src/util.cpp
                             src/AI.cpp)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(chess PRIVATE 
                            $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
                            $<IF:$<TARGET_EXISTS:SDL2::SDL2>, SDL2::SDL2, SDL2::SDL2-static>)
                             
find_package(SDL2_mixer CONFIG REQUIRED)
target_link_libraries(chess PRIVATE 
                             $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>, SDL2_mixer::SDL2_mixer, SDL2_mixer::SDL2_mixer-static>)
                             
find_package(SDL2_image CONFIG REQUIRED)
target_link_libraries(chess PRIVATE 
                             $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>, SDL2_image::SDL2_image, SDL2_image::SDL2_image-static>)
                             
target_include_directories(chess PRIVATE include)
target_link_libraries(chess)
                             
file(COPY resources DESTINATION ${PROJECT_BINARY_DIR})
