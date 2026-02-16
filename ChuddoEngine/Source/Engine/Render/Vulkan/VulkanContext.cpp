#include "Render/Vulkan/VulkanContext.h"
#include "Render/Vulkan/VulkanRenderer.h"
#include "Core/Engine.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
#include <cstring>

VulkanContext::VulkanContext ()
	{}

VulkanContext::~VulkanContext ()
	{}

bool VulkanContext::Initialize ( GLFWwindow * window )
	{
    // Use the provided native window. Do not create another GLFW window here
	if (!window)
		{
		LOG_FATAL ( "No native GLFW window provided to VulkanContext::Initialize" );
		return false;
		}

	Window = window;

	// Create Vulkan instance (minimal)
	VkApplicationInfo appInfo {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "ChuddoEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION ( 1, 0, 0 );
	appInfo.pEngineName = "ChuddoEngine";
	appInfo.engineVersion = VK_MAKE_VERSION ( 1, 0, 0 );
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Query required extensions from GLFW
	uint32_t glfwExtCount = 0;
	const char ** glfwExt = glfwGetRequiredInstanceExtensions ( &glfwExtCount );
	std::vector<const char *> extensions ( glfwExt, glfwExt + glfwExtCount );

	VkInstanceCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast< uint32_t >( extensions.size () );
	createInfo.ppEnabledExtensionNames = extensions.data ();
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance ( &createInfo, nullptr, &InstanceVk ) != VK_SUCCESS)
		{
		LOG_FATAL ( "Failed to create Vulkan instance" );
		return false;
		}

    // Create surface for the provided window
	if (glfwCreateWindowSurface ( InstanceVk, Window, nullptr, &Surface ) != VK_SUCCESS)
		{
		LOG_FATAL ( "Failed to create window surface" );
		return false;
		}

	LOG_INFO ( "VulkanRenderer initialized (basic)." );
	return true;
	}

void VulkanContext::Shutdown ()
	{
    // Log current handles to help diagnose shutdown ordering issues
	LOG_DEBUG("VulkanContext::Shutdown() - InstanceVk=", ( void * ) InstanceVk, " Surface=", (void*)Surface," Window = ",
			  (void*)Window);

	// Destroy Vulkan surface and instance if they exist. Do not touch GLFW window here
	// (the window owner is responsible for destroying it).
	if (Surface != VK_NULL_HANDLE)
		{
		// Destroy the surface first
		vkDestroySurfaceKHR(InstanceVk, Surface, nullptr);
		Surface = VK_NULL_HANDLE;
		}

	if (InstanceVk != VK_NULL_HANDLE)
		{
		// Destroy the Vulkan instance
		vkDestroyInstance(InstanceVk, nullptr);
		InstanceVk = VK_NULL_HANDLE;
		}

	LOG_DEBUG("Vulkan Context ShutDown");
	}

void VulkanContext::Render ( const RenderScene & scene )
	{
	glfwPollEvents (); // Poll GLFW events to update window state
	
	if (Window && glfwWindowShouldClose ( Window ))
		{
		LOG_DEBUG ( "[VULKAN RENDERER] Window closed send request exit..." );
		CEngine::Get ().RequestExit ();
		}

	// For now we'll just log periodically using the scene data
	static float timer = 0.f;
	timer += scene.DeltaTime;
	if (timer > 1.0f)
		{
		LOG_DEBUG ( "[VULKAN RENDERER] Render called with scene data" );
		timer = 0.f;
		}
	}
