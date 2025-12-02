cmake_minimum_required(VERSION 3.16)

project(GameEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration
set(ENGINE_NAME "GameEngine")

# Output directories - все собираем в одну папку независимо от конфигурации
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Для мультиконфигурационных сборок используем CMAKE_CFG_INTDIR
set(SHADER_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/Assets/Shaders)
set(ASSETS_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/Assets)

# External dependencies
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/thirdParty)

# Ищем glslangValidator
find_program(GLSLANG_VALIDATOR glslangValidator)
if(NOT GLSLANG_VALIDATOR)
    message(WARNING "glslangValidator not found. Shader compilation will be disabled.")
    set(SHADER_COMPILATION_ENABLED OFF)
else()
    set(SHADER_COMPILATION_ENABLED ON)
    message(STATUS "Found glslangValidator: ${GLSLANG_VALIDATOR}")
endif()

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
    ${CMAKE_SOURCE_DIR}/Include/Engine/Application
    ${CMAKE_SOURCE_DIR}/Include/Engine/Editor
    ${CMAKE_SOURCE_DIR}/Include/Engine/Core
    ${CMAKE_SOURCE_DIR}/Include/Engine/GamePlay
    ${CMAKE_SOURCE_DIR}/Include/Engine/Utils
    ${ASSIMP_INCLUDE_DIRS}
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

# Компиляция шейдеров
if(EXISTS ${CMAKE_SOURCE_DIR}/Assets/Shaders AND SHADER_COMPILATION_ENABLED)
    # Находим все файлы шейдеров
    file(GLOB_RECURSE SHADER_FILES
        ${CMAKE_SOURCE_DIR}/Assets/Shaders/*.vert
        ${CMAKE_SOURCE_DIR}/Assets/Shaders/*.frag
        ${CMAKE_SOURCE_DIR}/Assets/Shaders/*.tesc
        ${CMAKE_SOURCE_DIR}/Assets/Shaders/*.tese
        ${CMAKE_SOURCE_DIR}/Assets/Shaders/*.geom
        ${CMAKE_SOURCE_DIR}/Assets/Shaders/*.comp
    )
    
    if(SHADER_FILES)
        message(STATUS "Found ${SHADER_FILES} shader files for compilation")
        
        # Создаем целевые выходные файлы SPIR-V
        foreach(SHADER_FILE ${SHADER_FILES})
            # Получаем имя файла
            get_filename_component(SHADER_FILENAME ${SHADER_FILE} NAME)
            # Преобразуем имя для выходного файла
            string(REPLACE "." "_" OUTPUT_FILENAME ${SHADER_FILENAME})
            set(OUTPUT_FILE ${SHADER_OUTPUT_DIR}/${OUTPUT_FILENAME}.spv)
            
            # Команда компиляции шейдера
            add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
                COMMAND ${GLSLANG_VALIDATOR} -V "${SHADER_FILE}" -o "${OUTPUT_FILE}"
                COMMENT "Compiling shader: ${SHADER_FILENAME} -> ${OUTPUT_FILENAME}.spv"
            )
        endforeach()
    endif()
endif()

# Копирование ВСЕХ ассетов рекурсивно
if(EXISTS ${CMAKE_SOURCE_DIR}/Assets)
    message(STATUS "Copying all Assets from ${CMAKE_SOURCE_DIR}/Assets to ${ASSETS_OUTPUT_DIR}")
    
    # Рекурсивно копируем всю папку Assets со всеми подпапками
    add_custom_command(TARGET ${ENGINE_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying Assets folder..."
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${ASSETS_OUTPUT_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Assets"
            "${ASSETS_OUTPUT_DIR}"
        COMMENT "Copying all Assets recursively"
    )
    
    # Если шейдеры компилируются, удаляем исходные файлы шейдеров
    # оставляем только .spv файлы, которые будут скомпилированы отдельно
    if(SHADER_COMPILATION_ENABLED)
        add_custom_command(TARGET ${ENGINE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Removing source shader files..."
            COMMAND ${CMAKE_COMMAND} -E remove -f 
                "${ASSETS_OUTPUT_DIR}/Shaders/*.vert"
                "${ASSETS_OUTPUT_DIR}/Shaders/*.frag"
                "${ASSETS_OUTPUT_DIR}/Shaders/*.tesc"
                "${ASSETS_OUTPUT_DIR}/Shaders/*.tese"
                "${ASSETS_OUTPUT_DIR}/Shaders/*.geom"
                "${ASSETS_OUTPUT_DIR}/Shaders/*.comp"
            COMMENT "Removing source shader files (keeping only .spv)"
        )
    endif()
endif()

# Link libraries
target_link_libraries(${ENGINE_NAME}
    ${SDL3_LIBRARY}
    ${ASSIMP_LIBRARY}
    Vulkan::Vulkan
)

# Compiler settings
target_compile_options(${ENGINE_NAME} PRIVATE -Wall -Wextra)

# Для Windows добавляем дополнительные библиотеки
if(WIN32)
    target_link_libraries(${ENGINE_NAME}
        opengl32
        gdi32
        user32
        shell32
        advapi32
    )
endif()

# Display build information
message(STATUS "Building ${ENGINE_NAME}")