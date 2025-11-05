@echo off
echo Setting up dependencies...

mkdir thirdParty 2>nul
cd thirdParty

echo Cloning GLFW...
if not exist glfw (
    git clone https://github.com/glfw/glfw.git
) else (
    echo GLFW already exists
)

echo Cloning GLM...
if not exist glm (
    git clone https://github.com/g-truc/glm.git
) else (
    echo GLM already exists
)

echo.
echo Please install Vulkan SDK manually from: https://vulkan.lunarg.com/
echo After installation, update CMakeLists.txt with correct Vulkan paths
echo.
pause