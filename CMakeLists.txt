cmake_minimum_required(VERSION 3.10)
project(tg)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Optimierung + Strip (für kleine Binary)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")

# Binary im Projekt-Root ausgeben
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network   # 👈 HINZUGEFÜGT
        REQUIRED)

file(GLOB_RECURSE TOOL_SOURCES guiTools/*.cpp)

add_executable(tg main.cpp WindowOCR.cpp WindowOCR_hello.cpp DesktopSelection.cpp DiaSelectLang.cpp ${TOOL_SOURCES})

target_link_libraries(tg
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network   # 👈 HINZUGEFÜGT
        tesseract
        lept
)
