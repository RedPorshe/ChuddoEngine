#pragma once

class IWindow
	{
	public:
		virtual ~IWindow () = default;
		virtual bool Initialize ( int width, int height, const char* title ) = 0;
		virtual void Shutdown () = 0;
		virtual void PollEvents () = 0;
		virtual bool ShouldClose () const = 0;
		virtual void* GetNativeWindow () const = 0;
	};