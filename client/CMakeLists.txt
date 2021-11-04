project(client)

add_executable(client
    main.cc
    gameplay/game_object.cc
    gameplay/map_scene.cc
    gameplay/collider.cc
    gameplay/objects/player.cc
    gameplay/objects/bomb.cc
    utils/unique_id.cc
    core/assets.cc
    core/json.cc
    core/sprite.cc
    core/tilemap.cc
)

target_link_libraries(client raylib)
target_link_libraries(client cjson)
target_link_libraries(client colla)
target_link_libraries(client box2d)

target_include_directories(client PUBLIC ${CMAKE_SOURCE_DIR}/libs/raylib/src)
target_include_directories(client PUBLIC ${CMAKE_SOURCE_DIR}/libs/colla)
target_include_directories(client PUBLIC ${CMAKE_SOURCE_DIR}/libs/cjson)
target_include_directories(client PUBLIC ${CMAKE_SOURCE_DIR}/libs/box2d)
target_include_directories(client PUBLIC ${CMAKE_SOURCE_DIR}/client)