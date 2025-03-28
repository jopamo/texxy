cmake_minimum_required(VERSION 3.16)
project(Texxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(texxy
    src/texxy.cpp
    src/syntax-c.cpp
    src/editorwidget.cpp
    src/findreplacedialog.cpp
)

target_link_libraries(texxy
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

install(TARGETS texxy
    RUNTIME DESTINATION bin
)

install(FILES texxy.desktop
    DESTINATION share/applications
)

install(FILES texxy.png
    DESTINATION share/icons/hicolor/256x256/apps
)
