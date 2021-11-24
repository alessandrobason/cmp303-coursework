project(server)

add_executable(server
    main.cc
    gameplay/game_object.cc
    gameplay/map_scene.cc
    gameplay/menu_scene.cc
    gameplay/join_scene.cc
    gameplay/host_scene.cc
    gameplay/collider.cc
    gameplay/objects/player.cc
    gameplay/objects/bomb.cc
    gameplay/objects/explosion.cc
    gameplay/objects/crate.cc
    gameplay/objects/powerup.cc
    utils/unique_id.cc
    utils/base26.cc
    core/assets.cc
    core/json.cc
    core/sprite.cc
    core/tilemap.cc
    core/gui.cc
    core/scene.cc
    core/id.cc
    net/net.cc
    net/convert.cc
)

target_link_libraries(server raylib)
target_link_libraries(server cjson)
target_link_libraries(server colla)
target_link_libraries(server box2d)

target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/libs/raylib/src)
target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/libs/colla)
target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/libs/cjson)
target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/libs/box2d)
target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR}/server)