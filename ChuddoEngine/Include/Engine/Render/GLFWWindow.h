#pragma once
#include "CoreMinimal.h"
#include "Render/WindowInterface.h"

struct GLFWwindow;

class GLFWWindowPtr : public IWindow
	{
	public:
	virtual ~GLFWWindowPtr ();
	// Inherited via IWindow
	bool Initialize ( int width, int height, const char * title ) override;
	void Shutdown () override;
	void PollEvents () override;
	bool ShouldClose () const override;
	void * GetNativeWindow () const override;
	private :
		GLFWwindow * Window = nullptr;
		friend class IWindow;
	};
