cmake_minimum_required(VERSION 3.16)
project(ImageEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 5.15.2 (MinGW)
set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/mingw81_64/lib/cmake" CACHE PATH "Qt5 CMake prefix" FORCE)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 5.15.2 REQUIRED COMPONENTS Widgets Gui Core)

# Pure Qt5 build (no OpenCV)

# Sources (modular layout)
add_executable(ImageEditor WIN32
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/brightnesscontrastdialog.cpp
    src/ui/brightnesscontrastdialog.h
    src/core/ImageDocument.cpp
    src/core/ImageDocument.h
)

# Link libraries
target_link_libraries(ImageEditor PRIVATE
    Qt5::Widgets
    Qt5::Gui
    Qt5::Core
)

# Output directory
set_target_properties(ImageEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# windeployqt
set(WINDEPLOYQT_EXE "C:/Qt/5.15.2/mingw81_64/bin/windeployqt.exe")
if(EXISTS "${WINDEPLOYQT_EXE}")
    add_custom_command(TARGET ImageEditor POST_BUILD
        COMMAND "${WINDEPLOYQT_EXE}" --no-translations --no-compiler-runtime "$<TARGET_FILE:ImageEditor>"
        COMMENT "Running windeployqt to deploy Qt runtime files"
        VERBATIM
    )
endif()
