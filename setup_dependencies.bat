@echo off
echo Setting up dependencies...

mkdir thirdParty 2>nul
cd thirdParty

echo Cloning SDL2...
if not exist SDL (
    git clone https://github.com/libsdl-org/SDL.git
) else (
    echo SDL2 already exists
)

echo Cloning GLM...
if not exist glm (
    git clone https://github.com/g-truc/glm.git
) else (
    echo GLM already exists
)

echo Cloning ImGui...
if not exist imgui (
    git clone https://github.com/ocornut/imgui.git
) else (
    echo ImGui already exists
)

echo Cloning Assimp (for FBX, OBJ, and other model formats)...
if not exist assimp (
    git clone https://github.com/assimp/assimp.git
) else (
    echo Assimp already exists
)

echo.
echo Please install Vulkan SDK manually from: https://vulkan.lunarg.com/
echo After installation, update CMakeLists.txt with correct Vulkan paths
echo.
pause
