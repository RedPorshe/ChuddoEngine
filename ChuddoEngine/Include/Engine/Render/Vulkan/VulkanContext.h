#pragma once

#include <vulkan/vulkan.h>
struct GLFWwindow;
struct RenderScene;


class VulkanContext
	{
	public:
		VulkanContext ();
		~VulkanContext ();
		bool Initialize ( GLFWwindow* window );
		void Shutdown ();
		void Render ( const RenderScene & scene );
		VkInstance GetInstance () const { return InstanceVk; }
		VkSurfaceKHR GetSurface () const { return Surface; }
	private:
	
	
		VkInstance InstanceVk = VK_NULL_HANDLE;
		VkSurfaceKHR Surface = VK_NULL_HANDLE;
		GLFWwindow * Window = nullptr;
	};