cmake_minimum_required(VERSION 3.20)
project(GentooUpdateChecker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    DBus
)

find_package(KF6 REQUIRED COMPONENTS
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
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::DBus
    KF6::CoreAddons
    KF6::DBusAddons
    KF6::StatusNotifierItem
)

# Installation
install(TARGETS gentoo-update-checker DESTINATION bin)
install(FILES org.gentoo.UpdateChecker.desktop DESTINATION share/applications/autostart)
