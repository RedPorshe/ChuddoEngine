@echo off
echo Setting up dependencies...

mkdir thirdParty 2>nul
cd thirdParty

echo Cloning SDL3...
if not exist SDL (
    git clone https://github.com/libsdl-org/SDL.git
) else (
    echo SDL3 already exists
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
