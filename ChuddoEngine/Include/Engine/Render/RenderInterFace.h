#pragma once

#include "RenderInfo.h"

class IRenderer
	{
	public:
		virtual ~IRenderer () = default;
		virtual bool Initialize () = 0;
		virtual void Shutdown () = 0;

		// Передать все данные сцены за кадр в рендерер
		virtual void Render ( const RenderScene & scene ) = 0;
	};


