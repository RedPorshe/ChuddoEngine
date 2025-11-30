cmake_minimum_required(VERSION 4.1.2)

# Remove compiler reset - this causes warnings
# Instead, we use find_program for reliable search

project(GameEngine VERSION 1.0.0 LANGUAGES CXX)

# Reliable search for MinGW compilers
if(WIN32)
    find_program(MINGW_CXX_COMPILER 
        NAMES g++ c++ x86_64-w64-mingw32-g++ 
        PATHS "D:/mingw64/bin" "C:/mingw64/bin" 
        NO_DEFAULT_PATH
    )
    find_program(MINGW_C_COMPILER 
        NAMES gcc cc x86_64-w64-mingw32-gcc
        PATHS "D:/mingw64/bin" "C:/mingw64/bin"
        NO_DEFAULT_PATH
    )
    
    if(MINGW_CXX_COMPILER AND MINGW_C_COMPILER)
        set(CMAKE_CXX_COMPILER "${MINGW_CXX_COMPILER}" CACHE FILEPATH "C++ compiler" FORCE)
        set(CMAKE_C_COMPILER "${MINGW_C_COMPILER}" CACHE FILEPATH "C compiler" FORCE)
        message(STATUS "Using MinGW compilers: C=${MINGW_C_COMPILER}, CXX=${MINGW_CXX_COMPILER}")
        set(MINGW TRUE CACHE BOOL "Using MinGW compiler")
    else()
        message(WARNING "MinGW compilers not found in standard paths")
    endif()
endif()

# Add this after project()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")

# Define MINGW explicitly (alternative way)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND WIN32)
    set(MINGW TRUE CACHE BOOL "Using MinGW compiler")
    message(STATUS "MinGW compiler detected: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# Define macros depending on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
    message(STATUS "Debug build - _DEBUG defined")
else()
    add_compile_definitions(NDEBUG)
    message(STATUS "Release build - NDEBUG defined")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration
set(ENGINE_NAME "GameEngine")

# For MinGW, create separate folders for Debug and Release
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Debug)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Debug)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Debug)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release)
endif()

# External dependencies
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/thirdParty)

# SDL3
if(EXISTS ${EXTERNAL_DIR}/SDL/CMakeLists.txt)
    # Disable SDL tests and some features to speed up build
    set(SDL_TESTS OFF CACHE BOOL "Disable SDL tests" FORCE)
    if(MINGW)
        set(SDL_SHARED OFF CACHE BOOL "Disable SDL shared library for MinGW" FORCE)
        set(SDL_STATIC ON CACHE BOOL "Build SDL as static library for MinGW" FORCE)
        message(STATUS "Using bundled SDL3 static library for MinGW")
    else()
        set(SDL_SHARED ON CACHE BOOL "Build SDL as shared library" FORCE)
        set(SDL_STATIC OFF CACHE BOOL "Disable SDL static library" FORCE)
        message(STATUS "Using bundled SDL3 shared library")
    endif()
    set(SDL_DISABLE_INSTALL ON CACHE BOOL "Disable SDL install rules" FORCE)
    add_subdirectory(${EXTERNAL_DIR}/SDL)
    if(MINGW)
        set(SDL3_LIBRARY SDL3-static)
    else()
        set(SDL3_LIBRARY SDL3)
    endif()
    set(SDL3_INCLUDE_DIRS ${EXTERNAL_DIR}/SDL/include ${EXTERNAL_DIR}/SDL/include/SDL3)
    message(STATUS "Using bundled SDL3")
else()
    find_package(SDL3 REQUIRED)
    set(SDL3_LIBRARY SDL3)
    message(STATUS "Using system SDL3")
endif()

# ImGui
if(EXISTS ${EXTERNAL_DIR}/imgui)
    # Add ImGui sources manually for Vulkan backend
    set(IMGUI_SOURCES
        ${EXTERNAL_DIR}/imgui/imgui.cpp
        ${EXTERNAL_DIR}/imgui/imgui_draw.cpp
        ${EXTERNAL_DIR}/imgui/imgui_widgets.cpp
        ${EXTERNAL_DIR}/imgui/imgui_tables.cpp
        ${EXTERNAL_DIR}/imgui/backends/imgui_impl_sdl3.cpp
        ${EXTERNAL_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    )

    add_library(imgui STATIC ${IMGUI_SOURCES})
    target_include_directories(imgui PUBLIC
        ${EXTERNAL_DIR}/imgui
        ${EXTERNAL_DIR}/imgui/backends
        ${SDL3_INCLUDE_DIRS}
        ${Vulkan_INCLUDE_DIRS}
    )
    target_link_libraries(imgui PUBLIC ${SDL3_LIBRARY} Vulkan::Vulkan)
    set(IMGUI_LIBRARY imgui)
    message(STATUS "Using bundled ImGui with Vulkan backend")
else()
    find_package(imgui QUIET)
    if(NOT imgui_FOUND)
        message(FATAL_ERROR "ImGui not found! Please run setup_dependencies.bat or install ImGui manually")
    endif()
    set(IMGUI_LIBRARY imgui)
endif()

# GLM (header-only)
if(EXISTS ${EXTERNAL_DIR}/glm/CMakeLists.txt)
    add_subdirectory(${EXTERNAL_DIR}/glm)
    set(GLM_INCLUDE_DIRS ${EXTERNAL_DIR}/glm)
    message(STATUS "Using bundled GLM")
else()
    find_package(glm REQUIRED)
    message(STATUS "Using system GLM")
endif()

# Vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message(STATUS "Vulkan found: ${Vulkan_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Vulkan not found! Please install Vulkan SDK")
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Include
    ${GLM_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
    ${SDL3_INCLUDE_DIRS}
)

# Source files
file(GLOB_RECURSE SOURCES
    "Source/*.cpp"
    "Source/*/*.cpp"
)

# Create executable
add_executable(${ENGINE_NAME} ${SOURCES})

# Disable console in Release version (only for MinGW)
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND MINGW)
    target_link_options(${ENGINE_NAME} PRIVATE -mwindows)
    message(STATUS "Console disabled for Release build (MinGW)")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug" AND MINGW)
    message(STATUS "Console enabled for Debug build (MinGW)")
endif()

# Find glslangValidator
find_program(GLSLANG_VALIDATOR glslangValidator)

if(GLSLANG_VALIDATOR)
    # Find ALL shader files
    file(GLOB SHADER_FILES 
        "${CMAKE_SOURCE_DIR}/Assets/Shaders/*.vert"
        "${CMAKE_SOURCE_DIR}/Assets/Shaders/*.frag"
        "${CMAKE_SOURCE_DIR}/Assets/Shaders/*.comp"
        "${CMAKE_SOURCE_DIR}/Assets/Shaders/*.geom"
        "${CMAKE_SOURCE_DIR}/Assets/Shaders/*.tesc"
        "${CMAKE_SOURCE_DIR}/Assets/Shaders/*.tese"
    )
    
    set(SPV_FILES "")
    set(SHADER_DEPENDENCIES "")
    
    # Create rules for each shader
    foreach(SHADER_FILE ${SHADER_FILES})
        # Get filename without extension
        get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
        get_filename_component(SHADER_EXT ${SHADER_FILE} EXT)
        
        # Determine shader type by extension
        if(SHADER_EXT STREQUAL ".vert")
            set(SPV_EXT "_vert.spv")
        elseif(SHADER_EXT STREQUAL ".frag")
            set(SPV_EXT "_frag.spv")
        elseif(SHADER_EXT STREQUAL ".comp")
            set(SPV_EXT "_comp.spv")
        elseif(SHADER_EXT STREQUAL ".geom")
            set(SPV_EXT "_geom.spv")
        elseif(SHADER_EXT STREQUAL ".tesc")
            set(SPV_EXT "_tesc.spv")
        elseif(SHADER_EXT STREQUAL ".tese")
            set(SPV_EXT "_tese.spv")
        else()
            message(WARNING "Unknown shader extension: ${SHADER_EXT} for file ${SHADER_FILE}")
            continue()
        endif()
        
        # Define output file
        set(SPV_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders/${SHADER_NAME}${SPV_EXT}")
        
        # Create compilation rule
        add_custom_command(
            OUTPUT ${SPV_FILE}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders"
            COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER_FILE} -o ${SPV_FILE}
            DEPENDS ${SHADER_FILE}
            COMMENT "Compiling shader: ${SHADER_FILE} -> ${SPV_FILE}"
        )
        
        list(APPEND SPV_FILES ${SPV_FILE})
        list(APPEND SHADER_DEPENDENCIES ${SHADER_FILE})
        
        message(STATUS "Shader compilation rule added: ${SHADER_FILE} -> ${SPV_FILE}")
    endforeach()
    
    # Add target for compiling all shaders
    add_custom_target(CompileAllShaders ALL DEPENDS ${SPV_FILES})
    add_dependencies(${ENGINE_NAME} CompileAllShaders)
    
    message(STATUS "SPIR-V shader compilation enabled for all shaders")
    message(STATUS "Found shader files: ${SHADER_FILES}")
else()
    message(WARNING "glslangValidator not found - shaders must be precompiled")
endif()

# Copy Assets folder (without shader sources, only compiled)
if(EXISTS ${CMAKE_SOURCE_DIR}/Assets)
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Assets"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets"
    )
    
    # Remove shader sources after copying (keep only .spv)
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders/*.vert"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders/*.frag"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders/*.comp"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders/*.geom"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders/*.tesc"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders/*.tese"
        COMMENT "Removing shader source files (keeping only .spv)"
    )
    
    message(STATUS "Assets directory will be copied")
else()
    message(WARNING "Assets directory not found at: ${CMAKE_SOURCE_DIR}/Assets")
endif()

# Link libraries
target_link_libraries(${ENGINE_NAME}
    ${SDL3_LIBRARY}
    ${IMGUI_LIBRARY}
    Vulkan::Vulkan
)

# Static linking for MinGW
if(MINGW)
    target_link_options(${ENGINE_NAME} PRIVATE 
        -static
        -static-libgcc 
        -static-libstdc++
    )
    message(STATUS "Static linking enabled for MinGW")
endif()

# Compiler specific settings
target_compile_options(${ENGINE_NAME} PRIVATE -Wall -Wextra)

# Copy DLLs for Windows
if(WIN32)
    # Copy only Vulkan DLL
    find_file(VULKAN_DLL vulkan-1.dll 
        PATHS "C:/VulkanSDK/*/Bin"
        DOC "Path to Vulkan DLL"
    )
    if(VULKAN_DLL)
        add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${VULKAN_DLL}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        message(STATUS "Vulkan DLL will be copied")
    endif()
endif()

# Remove static libraries after build
add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libglm.a"
    COMMENT "Removing static library files"
)

# Display build information
message(STATUS "Building ${ENGINE_NAME}")