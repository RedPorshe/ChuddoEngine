#include "Render/VulkanRenderer.h"

VulkanRenderer::VulkanRenderer ()
	{}

VulkanRenderer::~VulkanRenderer ()
	{}

bool VulkanRenderer::Initialize ()
	{
	return false;
	}

void VulkanRenderer::Shutdown ()
	{}

void VulkanRenderer::Render ( const RenderScene & scene )
	{	
	static float timer = 0.f;
	timer += scene.DeltaTime; 
	if (timer > 1.0f)
		{
		LOG_DEBUG ( "[VULKAN RENDERER] Render called with scene data" );
		timer = 0.f; // Reset timer to log every second
		}
	}
