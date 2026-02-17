#pragma once

#include "Render/RenderInterFace.h"
#include "Render/Vulkan/VulkanContext.h"
#include <memory>

class VulkanContext;
class IWindow;
struct VulkanContextInfo;

class VulkanRenderer : public IRenderer
	{
	public:
		VulkanRenderer ( const VulkanContextInfo & info );
		virtual ~VulkanRenderer () override;
		virtual bool Initialize () override;
		virtual void Shutdown () override;
		virtual void Render ( const RenderScene & scene ) override;
		std::unique_ptr <IWindow> & GetWindowPtr () { return Window; }
	private:
		std::unique_ptr<VulkanContext> Context = nullptr;
		std::unique_ptr< IWindow> Window = nullptr;
		// Inherited via IRenderer
		void * GetWindow () const override;
		void * RendererWindow = nullptr;
      VulkanContextInfo Info ;
	};