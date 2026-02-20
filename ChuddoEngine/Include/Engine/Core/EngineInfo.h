#pragma once
#include <string>

struct WindowInfo
	{
    std::string Title = "Game Window";
    int Width = 1280;
    int Height = 720;
    bool Fullscreen = false;
    bool Resizable = true;
    bool VSync = true;  // Будет управляться через Vulkan (present mode)
    int PosX = -1;  // -1 означает центр экрана
    int PosY = -1;

    // Параметры Vulkan
    bool UseVulkan = true;  // Флаг использования Vulkan
	};