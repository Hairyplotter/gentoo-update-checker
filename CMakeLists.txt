cmake_minimum_required(VERSION 3.16)
project(GentooUpdateChecker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    DBus
)

find_package(KF5 REQUIRED COMPONENTS
    CoreAddons
    DBusAddons
    StatusNotifierItem
)

set(SOURCE_FILES
    src/main.cpp
    src/updatechecker.cpp
    src/updatechecker.h
    src/systemtraywidget.cpp
    src/systemtraywidget.h
)

add_executable(gentoo-update-checker ${SOURCE_FILES})

target_link_libraries(gentoo-update-checker
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::DBus
    KF5::CoreAddons
    KF5::DBusAddons
    KF5::StatusNotifierItem
)

# Installation
install(TARGETS gentoo-update-checker DESTINATION bin)
install(FILES org.gentoo.UpdateChecker.desktop DESTINATION share/applications/autostart)
