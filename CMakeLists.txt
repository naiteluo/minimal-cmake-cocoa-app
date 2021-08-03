cmake_minimum_required(VERSION 3.19)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
set(CMAKE_CXX_STANDARD 14)

# in case to see more warning
set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O4 -DNDEBUG")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG")

project(CocoaApp)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# fine cocoa and opengl
find_library(COCOA_LIBRARY Cocoa required)
find_library(OPENGL_LIBRARY OpenGL required)

add_executable(CocoaApp
        MACOSX_BUNDLE
        CocoaApplication.mm
        AppDelegate.m
        WindowDelegate.m
        CustomizedView.mm
        GraphicsManager.cpp
        ${PROJECT_SOURCE_DIR}/External/GL/src/glad.c
        )

target_include_directories(CocoaApp PRIVATE External/GL/include)
target_link_libraries(CocoaApp ${COCOA_LIBRARY} ${OPENGL_LIBRARY} Eigen3::Eigen)