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
    ui/towerpanel.cpp
    ui/towerpanel.h
    ui/towerselectionpopup.cpp
    ui/towerselectionpopup.h
    ui/mainmenuwidget.ui
    ui/mainmenuwidget.cpp
    ui/mainmenuwidget.h
    ui/levelselectwidget.ui
    ui/levelselectwidget.cpp
    ui/levelselectwidget.h
    ui/gameresultwidget.ui
    ui/gameresultwidget.cpp
    ui/gameresultwidget.h
        ui/gameoverlay.cpp
        ui/gameoverlay.h
        ui/gameoverlay.ui
        ui/gamehud.cpp
        ui/gamehud.h
        ui/gamehud.ui
        ui/tutorialarrow.cpp
        ui/tutorialarrow.h
        ui/tutorialcontroller.cpp
        ui/tutorialcontroller.h
    game/enemies/enemy.cpp
    game/enemies/enemy.h
    game/towers/tower.cpp
    game/towers/tower.h
    game/wave.cpp
    game/wave.h
    game/spatialgrid.cpp
    game/spatialgrid.h
    game/gamecontroller.cpp
    game/gamecontroller.h
    game/towermanager.cpp
    game/towermanager.h
    ui/gamerenderer.cpp
    ui/gamerenderer.h
    ui/inputhandler.cpp
    ui/inputhandler.h
    ui/panelcontroller.cpp
    ui/panelcontroller.h
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
    game/bullets/arrowbullet.cpp
    game/bullets/arrowbullet.h
    game/bullets/cannonbullet.cpp
    game/bullets/cannonbullet.h
    game/bullets/icebullet.cpp
    game/bullets/icebullet.h
    game/bullets/poisonbullet.cpp
    game/bullets/poisonbullet.h
    game/bullets/lightningbullet.cpp
    game/bullets/lightningbullet.h
    game/bullets/bulletfactory.cpp
    game/bullets/bulletfactory.h
    game/markers/marker.h
    game/markers/slowmarker.cpp
    game/markers/slowmarker.h
    game/markers/poisonmarker.cpp
    game/markers/poisonmarker.h
    game/markers/markerfactory.cpp
    game/markers/markerfactory.h
    game/markers/freezemarker.cpp
    game/markers/freezemarker.h
    game/cellentities.h
    game/config/datamanager.cpp
    game/config/datamanager.h
    game/obstacles/obstacle.cpp
    game/obstacles/obstacle.h
    game/obstacles/treeobstacle.cpp
    game/obstacles/treeobstacle.h
    game/obstacles/statueobstacle.cpp
    game/obstacles/statueobstacle.h
    game/obstacles/obstaclefactory.cpp
    game/obstacles/obstaclefactory.h
)

if(WIN32)
    list(APPEND PROJECT_SOURCES app.rc)
endif()
if(APPLE)
    list(APPEND PROJECT_SOURCES assets/ui/icon.icns)
endif()

# Watch config JSON files for changes (trigger re-configure on edit)
file(GLOB_RECURSE CONFIG_FILES CONFIGURE_DEPENDS "config/*.json")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CarrotDefense
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CONFIG_FILES}
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

if(APPLE)
    set_target_properties(CarrotDefense PROPERTIES
        MACOSX_BUNDLE_ICON_FILE icon.icns
    )
    set_source_files_properties(assets/ui/icon.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()

# Copy config directory to build output so the EXE can find game.json
add_custom_command(TARGET CarrotDefense POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/config"
    "$<TARGET_FILE_DIR:CarrotDefense>/config"
    COMMENT "Copying config/ to build output..."
)

# Copy assets directory to build output so the EXE can find textures
add_custom_command(TARGET CarrotDefense POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:CarrotDefense>/assets"
    COMMENT "Copying assets/ to build output..."
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CarrotDefense)
endif()
