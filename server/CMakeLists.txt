project(server)

add_executable(server
    src/main.cc
    src/collider.cc
    utils/wait.cc
)

target_link_libraries(server cjson)
target_link_libraries(server colla)
target_link_libraries(server box2d)

if(MSVC)
    target_link_libraries(server winmm)
else()
endif()


target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/libs/colla)
target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/libs/cjson)
target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/libs/box2d)
target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/server)