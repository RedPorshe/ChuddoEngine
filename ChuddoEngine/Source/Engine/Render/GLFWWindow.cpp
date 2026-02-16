#include "Render/GLFWWindow.h"
#include <GLFW/glfw3.h>


bool GLFWWindowPtr::Initialize ( int width, int height, const char * title )
    {
    if (!glfwInit ())
        {
        LOG_ERROR ( "Failed to initialize GLFW" );
        return false;
        }

    glfwWindowHint ( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint ( GLFW_RESIZABLE, GLFW_FALSE );

    Window = glfwCreateWindow ( width, height, title, nullptr, nullptr );
    if (Window == nullptr)
        {
        LOG_ERROR ( "Failed to create GLFW window" );
        return false;
        }
    LOG_DEBUG ( "GLFW window created successfully" );
    return true;
    }

void GLFWWindowPtr::Shutdown ()
    {
    glfwDestroyWindow ( Window );
    Window = nullptr;
    glfwTerminate ();
    }

void GLFWWindowPtr::PollEvents ()
    {
    glfwPollEvents ();
    }

bool GLFWWindowPtr::ShouldClose () const
    {
    return glfwWindowShouldClose(Window);
    }

void * GLFWWindowPtr::GetNativeWindow () const
    {
    return Window;
    }
