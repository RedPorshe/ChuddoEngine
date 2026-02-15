#pragma once

#include "RenderInterFace.h"


class VulkanRenderer : public IRenderer
	{
	public:
		VulkanRenderer ();
		virtual ~VulkanRenderer () override;
		virtual bool Initialize () override;
		virtual void Shutdown () override;
		virtual void Render ( const RenderScene & scene ) override;
	private:
		//stub
	};