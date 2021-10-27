cmake_minimum_required(VERSION 3.16)
project(online_game)

add_subdirectory(libs)
add_subdirectory(src)

target_include_directories(online_game PUBLIC libs/raylib/src)
target_include_directories(online_game PUBLIC libs/colla)