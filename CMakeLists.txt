cmake_minimum_required(VERSION 3.16)

project(chess VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# 30,000,000 steps, dont know what the g++ or MSVC equivalent is
add_compile_options(-fconstexpr-steps=30000000)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick Multimedia Qml Widgets Positioning)

qt_standard_project_setup()

qt_add_executable(chess
  src/main.cpp
)

target_include_directories(chess PRIVATE include)

set_source_files_properties(qml/style/Style.qml PROPERTIES
  QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(chess
  URI chess2
  VERSION 1.0
  NO_RESOURCE_TARGET_PATH
  RESOURCE_PREFIX "/"

  QML_FILES qml/Main.qml
            qml/AppContent.qml
            qml/NavStack.qml
            qml/Home.qml
            qml/Board.qml
            qml/HomeRightPane.qml
            qml/HomeLeftPane.qml
            qml/MainMenu.qml
            qml/DefaultText.qml
            qml/PillButton.qml
            qml/style/Style.qml

  RESOURCES images/pawn_white.png
            images/pawn_black.png
            images/knight_white.png
            images/knight_black.png
            images/bishop_white.png
            images/bishop_black.png
            images/rook_white.png
            images/rook_black.png
            images/queen_white.png
            images/queen_black.png
            images/king_white.png
            images/king_black.png
            images/circle.png
            sounds/game_end.mp3
            sounds/illegal_move.mp3
            sounds/move.mp3

  SOURCES include/BoardModel.h
          include/BoardManager.hxx
          include/util.hxx
          include/MoveGen.hxx
          include/AI.hxx
          include/MoveModel.h
          src/BoardModel.cpp
          src/util.cpp
          src/BoardManager.cpp
          src/MoveGen.cpp
          src/AI.cpp
          src/MoveModel.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(chess PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appchess-qt
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(chess
  PRIVATE Qt6::Quick Qt6::Multimedia Qt6::Positioning Qt6::Qml Qt6::Widgets
)

include(GNUInstallDirs)

install(TARGETS chess
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
