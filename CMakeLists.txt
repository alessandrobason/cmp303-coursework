cmake_minimum_required(VERSION 3.16)
project(online_game)

add_subdirectory(libs)
add_subdirectory(client)
# add_subdirectory(server)

configure_file(config.json config.json COPYONLY)
file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})