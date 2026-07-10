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
)

# Installation
install(TARGETS gentoo-update-checker DESTINATION bin)
install(FILES org.gentoo.UpdateChecker.desktop DESTINATION share/applications/autostart)
