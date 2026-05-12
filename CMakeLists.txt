cmake_minimum_required(VERSION 3.21)

project(ResistanceCoupQt VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)
qt_standard_project_setup(REQUIRES 6.5)

if(COMMAND qt_policy)
    qt_policy(SET QTP0004 NEW)
endif()

add_library(coup_core STATIC
    Card.cpp
    Deck.cpp
    GameEngine.cpp
    GameEngine.h
    GameState.h
    Player.cpp
    randoms.cpp
)

set_target_properties(coup_core PROPERTIES
    AUTOMOC OFF
    AUTORCC OFF
    AUTOUIC OFF
)

target_include_directories(coup_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

qt_add_executable(resistance_coup_gui
    MACOSX_BUNDLE
    qt/main.cpp
    qt/GameController.cpp
    qt/GameController.h
)

target_link_libraries(resistance_coup_gui
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        coup_core
)

target_include_directories(resistance_coup_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/qt
)

qt_add_qml_module(resistance_coup_gui
    URI ResistanceCoup
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/StartScreen.qml
        qml/GameScreen.qml
        qml/BorderedButton.qml
)

install(TARGETS resistance_coup_gui
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
