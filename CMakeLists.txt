cmake_minimum_required(VERSION 3.20)
project(GameEngine VERSION 1.0.0 LANGUAGES CXX)

# Добавьте это после project()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Определяем макросы в зависимости от типа сборки
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

# Для MinGW создаем отдельные папки для Debug и Release
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

# GLFW
if(EXISTS ${EXTERNAL_DIR}/glfw/CMakeLists.txt)
    add_subdirectory(${EXTERNAL_DIR}/glfw)
    set(GLFW_LIBRARY glfw)
    message(STATUS "Using bundled GLFW")
else()
    find_package(glfw3 REQUIRED)
    set(GLFW_LIBRARY glfw)
    message(STATUS "Using system GLFW")
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
)

# Source files
file(GLOB_RECURSE SOURCES
    "Source/*.cpp"
    "Source/*/*.cpp"
)

# Create executable
add_executable(${ENGINE_NAME} ${SOURCES})

# Отключаем консоль в Release версии (только для MinGW)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(${ENGINE_NAME} PRIVATE -mwindows)
    message(STATUS "Console disabled for Release build (MinGW)")
else()
    message(STATUS "Console enabled for Debug build (MinGW)")
endif()

# Найдите glslangValidator
find_program(GLSLANG_VALIDATOR glslangValidator)

if(GLSLANG_VALIDATOR)
    # Находим ВСЕ файлы шейдеров
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
    
    # Создаем правила для каждого шейдера
    foreach(SHADER_FILE ${SHADER_FILES})
        # Получаем имя файла без расширения
        get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
        get_filename_component(SHADER_EXT ${SHADER_FILE} EXT)
        
        # Определяем тип шейдера по расширению
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
        
        # Определяем выходной файл
        set(SPV_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets/Shaders/${SHADER_NAME}${SPV_EXT}")
        
        # Создаем правило компиляции
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
    
    # Добавляем цель для компиляции всех шейдеров
    add_custom_target(CompileAllShaders ALL DEPENDS ${SPV_FILES})
    add_dependencies(${ENGINE_NAME} CompileAllShaders)
    
    message(STATUS "SPIR-V shader compilation enabled for all shaders")
    message(STATUS "Found shader files: ${SHADER_FILES}")
else()
    message(WARNING "glslangValidator not found - shaders must be precompiled")
endif()

# Копируем Assets папку (без исходников шейдеров, только скомпилированные)
if(EXISTS ${CMAKE_SOURCE_DIR}/Assets)
    add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Assets"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Assets"
    )
    
    # Удаляем исходники шейдеров после копирования (оставляем только .spv)
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
    ${GLFW_LIBRARY}
    Vulkan::Vulkan
)

# Статическая линковка для MinGW
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
    # Копируем только Vulkan DLL
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

# Убираем статические библиотеки после сборки
add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libglfw3.a"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libglm.a"
    COMMENT "Removing static library files"
)

# Display build information
message(STATUS "Building ${ENGINE_NAME}")