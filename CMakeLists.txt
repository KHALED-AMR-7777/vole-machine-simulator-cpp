cmake_minimum_required(VERSION 3.16)
project(VoleSimulatorGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# The team's simulator classes (everything except Menu and the console main).
set(CORE_SOURCES
    CPU.cpp
    Machine.cpp
    Memory.cpp
    Program_Loader.cpp
    Screen.cpp
)
# The Registers file was first called Registery.cpp; accept either name.
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Registers.cpp)
    list(APPEND CORE_SOURCES Registers.cpp)
else()
    list(APPEND CORE_SOURCES Registery.cpp)
endif()

add_executable(vole_gui
    gui/main_gui.cpp
    gui/MainWindow.cpp
    gui/MainWindow.h
    ${CORE_SOURCES}
)

target_include_directories(vole_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
)

target_link_libraries(vole_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(WIN32)
    # no black console window behind the GUI
    set_target_properties(vole_gui PROPERTIES WIN32_EXECUTABLE ON)
endif()
