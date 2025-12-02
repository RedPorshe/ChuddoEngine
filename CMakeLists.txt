cmake_minimum_required(VERSION 3.16)

project(GameEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration
set(ENGINE_NAME "GameEngine")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# External dependencies
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/thirdParty)

# SDL3
if(EXISTS ${EXTERNAL_DIR}/SDL/CMakeLists.txt)
    set(SDL_TESTS OFF CACHE BOOL "Disable SDL tests" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "Disable SDL shared library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build SDL as static library" FORCE)
    set(SDL_DISABLE_INSTALL ON CACHE BOOL "Disable SDL install rules" FORCE)
    add_subdirectory(${EXTERNAL_DIR}/SDL)
    set(SDL3_LIBRARY SDL3-static)
    set(SDL3_INCLUDE_DIRS ${EXTERNAL_DIR}/SDL/include ${EXTERNAL_DIR}/SDL/include/SDL3)
else()
    find_package(SDL3 REQUIRED)
    set(SDL3_LIBRARY SDL3)
endif()

# Vulkan
find_package(Vulkan REQUIRED)

# Assimp
if(EXISTS ${EXTERNAL_DIR}/assimp/CMakeLists.txt)
    add_subdirectory(${EXTERNAL_DIR}/assimp)
    set(ASSIMP_LIBRARY assimp)
    set(ASSIMP_INCLUDE_DIRS ${EXTERNAL_DIR}/assimp/include)
else()
    find_package(assimp REQUIRED)
    set(ASSIMP_LIBRARY assimp)
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Include
    ${CMAKE_SOURCE_DIR}/Include/Enine/Application
    ${CMAKE_SOURCE_DIR}/Include/Enine/Editor
    ${CMAKE_SOURCE_DIR}/Include/Enine/Core
    ${CMAKE_SOURCE_DIR}/Include/Enine/GamePlay
    ${CMAKE_SOURCE_DIR}/Include/Enine/Utils
    ${ASSIMP_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
    ${SDL3_INCLUDE_DIRS}
)

# Source files
file(GLOB_RECURSE SOURCES
    "Source/*.cpp"
    "Source/*/*.cpp"
    "Include/Engine/Editor/UI/SlateInputComponent.h"
)

# Create executable
add_executable(${ENGINE_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${ENGINE_NAME}
    ${SDL3_LIBRARY}
    ${ASSIMP_LIBRARY}
    Vulkan::Vulkan
)

# SDL is now linked statically, no need to copy DLL

# Compiler settings
target_compile_options(${ENGINE_NAME} PRIVATE -Wall -Wextra)

# Copy Assets folder
if(EXISTS ${CMAKE_SOURCE_DIR}/Assets)
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Assets"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets"
    )
endif()

# Display build information
message(STATUS "Building ${ENGINE_NAME}")
