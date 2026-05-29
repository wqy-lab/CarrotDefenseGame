cmake_minimum_required(VERSION 3.16)

project(CarrotDefense VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    ui/mainwindow.cpp
    ui/mainwindow.h
    ui/mainwindow.ui
    ui/gamescene.cpp
    ui/gamescene.h
    ui/mainmenuwidget.ui
    ui/mainmenuwidget.cpp
    ui/mainmenuwidget.h
    ui/levelselectwidget.ui
    ui/levelselectwidget.cpp
    ui/levelselectwidget.h
    ui/gameresultwidget.ui
    ui/gameresultwidget.cpp
    ui/gameresultwidget.h
    game/enemies/enemy.cpp
    game/enemies/enemy.h
    game/towers/tower.cpp
    game/towers/tower.h
    game/wave.cpp
    game/wave.h
    game/enemies/enemyfactory.cpp
    game/enemies/enemyfactory.h
    game/node.h
    game/pathfinder.cpp
    game/pathfinder.h
    game/enemies/normalenemy.cpp
    game/enemies/normalenemy.h
    game/enemies/fastenemy.cpp
    game/enemies/fastenemy.h
    game/enemies/tankenemy.cpp
    game/enemies/tankenemy.h
    game/enemies/bossenemy.cpp
    game/enemies/bossenemy.h
    game/enemies/swarmenemy.cpp
    game/enemies/swarmenemy.h
    game/towers/remotetower.cpp
    game/towers/remotetower.h
    game/towers/meleetower.cpp
    game/towers/meleetower.h
    game/towers/arrowtower.cpp
    game/towers/arrowtower.h
    game/towers/canntower.cpp
    game/towers/canntower.h
    game/towers/icetower.cpp
    game/towers/icetower.h
    game/towers/poisontower.cpp
    game/towers/poisontower.h
    game/towers/lighttower.cpp
    game/towers/lighttower.h
    game/towers/suntower.cpp
    game/towers/suntower.h
    game/towers/towerfactory.cpp
    game/towers/towerfactory.h
    game/bullets/bullet.cpp
    game/bullets/bullet.h
    game/bullets/normalbullet.cpp
    game/bullets/normalbullet.h
    game/bullets/bulletfactory.cpp
    game/bullets/bulletfactory.h
    game/config/datamanager.cpp
    game/config/datamanager.h
)

# Watch config JSON files for changes (trigger re-configure on edit)
file(GLOB_RECURSE CONFIG_FILES CONFIGURE_DEPENDS "config/*.json")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CarrotDefense
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(CarrotDefense SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(CarrotDefense
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(CarrotDefense PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Copy config directory to build output so the EXE can find game.json
add_custom_command(TARGET CarrotDefense POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/config"
    "$<TARGET_FILE_DIR:CarrotDefense>/config"
    COMMENT "Copying config/ to build output..."
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CarrotDefense)
endif()
