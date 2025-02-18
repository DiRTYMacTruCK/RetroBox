cmake_minimum_required(VERSION 3.16)

project(RetroBox VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

qt_add_executable(RetroBox
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    resources.qrc
)

# Define target properties for Android with Qt 6
# set_property(TARGET RetroBox APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#              ${CMAKE_CURRENT_SOURCE_DIR}/android)

# Link Qt6 libraries
target_link_libraries(RetroBox PRIVATE Qt6::Widgets Qt6::Multimedia)

set_target_properties(RetroBox PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RetroBox
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RetroBox
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(RetroBox)
