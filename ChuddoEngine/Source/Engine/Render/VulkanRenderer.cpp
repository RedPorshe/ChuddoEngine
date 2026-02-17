#include "Render/Vulkan/VulkanRenderer.h"
#include "Render/Vulkan/VulkanContext.h"
#include "Render/GLFWWindow.h"

#include "Core/Engine.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
#include <cstring>





VulkanRenderer::VulkanRenderer ( const VulkanContextInfo & info ) : Info ( info )
	{
	}

VulkanRenderer::~VulkanRenderer ()
	{
	Shutdown ();
	}

bool VulkanRenderer::Initialize ()
	{
	Context = std::make_unique<VulkanContext> ();
	Window = std::make_unique< GLFWWindowPtr> ();
	if (!Window->Initialize ( Info.WindowWidth, Info.WindowHeight, Info.AppName.c_str() ))
		{
		LOG_ERROR ( "Failed to initialize GLFW window for VulkanRenderer" );
		return false;
		}
	RendererWindow = Window->GetNativeWindow ();
	GLFWwindow * nativeWindow = static_cast< GLFWwindow * >( RendererWindow );
	if (!nativeWindow)
		{
		LOG_ERROR ( "Failed to get native window handle for Vulkan initialization" );
		return false;
		}
    if (!Context->Initialize ( &Info, nativeWindow ))
		{
		LOG_ERROR ( "Failed to initialize Vulkan context" );
		return false;
		}
	return true;
	}

void VulkanRenderer::Shutdown ()
	{
    LOG_DEBUG ( "VulkanRenderer::Shutdown() - shutting down context" );
	if (Context)
		{
		Context->Shutdown ();
		}
	LOG_DEBUG ( "VulkanRenderer::Shutdown() - shutting down window" );
	if (Window)
		{
		Window->Shutdown ();
		}
	LOG_DEBUG ( "VulkanRenderer::Shutdown() - complete" );
	}

void VulkanRenderer::Render ( const RenderScene & scene )
	{
	Context->Render ( scene );
	}

void * VulkanRenderer::GetWindow () const
	{
	return RendererWindow;
	}
