cmake_minimum_required(VERSION 3.18)

project(chess VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick Multimedia Qml Widgets Positioning)

set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set(QML_IMPORT_PATH "${QT_QML_OUTPUT_DIRECTORY}" CACHE STRING "" FORCE)

file(GLOB_RECURSE QmlFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} qml/*.qml qml/*.js)
file(GLOB_RECURSE Assets RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} images/*.png images/*.svg sounds/*.mp3)
file(GLOB_RECURSE SourceFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file(GLOB_RECURSE IncludeFiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} include/*.h include/*.hxx)

qt_add_executable(${PROJECT_NAME} MACOSX_BUNDLE)

target_include_directories(${PROJECT_NAME} PRIVATE include)

set_source_files_properties(qml/style/Style.qml PROPERTIES
  QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(${PROJECT_NAME}
  URI Sukless
  VERSION 1.0
  NO_RESOURCE_TARGET_PATH
  QML_FILES ${QmlFiles}
  RESOURCES ${Assets}
  SOURCES ${IncludeFiles}
          ${SourceFiles}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER sukless2
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(chess
  PRIVATE Qt6::Quick Qt6::Multimedia Qt6::Positioning Qt6::Qml Qt6::Widgets
)

install(TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
