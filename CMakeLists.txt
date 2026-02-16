cmake_minimum_required(VERSION 3.16)
project(Tucil1_13524017 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui)
qt_standard_project_setup()

add_executable(Tucil1_13524017
    src/main.cpp
    src/MainWindow.h src/MainWindow.cpp
    src/BoardWidget.h src/BoardWidget.cpp
    src/Puzzle.h src/Puzzle.cpp
    src/Solver.h src/Solver.cpp
    src/ImageIO.h src/ImageIO.cpp
)

target_link_libraries(Tucil1_13524017 PRIVATE Qt6::Widgets Qt6::Gui)

# Optional: nicer warnings
if (MSVC)
  target_compile_options(Tucil1_13524017 PRIVATE /W4)
else()
  target_compile_options(Tucil1_13524017 PRIVATE -Wall -Wextra -Wpedantic)
endif()
