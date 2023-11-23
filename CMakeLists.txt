cmake_minimum_required(VERSION 3.16)

project(chess-qt VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick Multimedia Qml Widgets Positioning)

qt_standard_project_setup()

qt_add_executable(appchess-qt
    src/main.cpp
    images.qrc
)
target_include_directories(appchess-qt PRIVATE include)


qt_add_qml_module(appchess-qt
    URI chess-qt
    VERSION 1.0
    QML_FILES qml/Main.qml
    QML_FILES qml/AppContent.qml
    QML_FILES qml/NavStack.qml
    QML_FILES qml/Home.qml
    QML_FILES qml/Board.qml
    QML_FILES qml/HomeRightPane.qml
    QML_FILES qml/HomeLeftPane.qml
    SOURCES include/BoardModel.h
    SOURCES include/Game.hxx
    SOURCES include/BoardManager.hxx
    SOURCES include/util.hxx
    SOURCES include/MoveGen.hxx
    SOURCES include/BoardConstants.hxx
    SOURCES include/AI.hxx
    SOURCES src/BoardModel.cpp
    SOURCES src/util.cpp
    SOURCES src/BoardManager.cpp
    SOURCES src/MoveGen.cpp
    SOURCES src/AI.cpp
    SOURCES src/Game.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appchess-qt PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appchess-qt
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appchess-qt
    PRIVATE Qt6::Quick Qt6::Multimedia Qt6::Positioning Qt6::Qml Qt6::Widgets
)

include(GNUInstallDirs)
install(TARGETS appchess-qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
