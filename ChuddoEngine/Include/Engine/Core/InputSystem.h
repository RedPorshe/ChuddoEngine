#pragma once
#include "CoreMinimal.h"
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>
#include <vector>
#include <glm/glm.hpp>

// Forward declarations
class CPlayerController;
class CInputComponent;
struct FEngineInfo;

using InputActionDelegate = std::function<void ( float )>;
using InputAxisDelegate = std::function<void ( float )>;

class CInputSystem
    {
    public:
        // Singleton access
        static CInputSystem * GetInstance ();

        CInputSystem ( FEngineInfo & info );
        virtual ~CInputSystem ();

        // System overrides
        bool Initialize ( FEngineInfo & info );
        void ShutdownSystem ();
        void Update ( float DeltaTime );
        bool IsInitialized () const { return bIsInitialized; }

        // GLFW window association
        GLFWwindow * GetWindow () const;
        void SetWindow ( GLFWwindow * window ) {
            m_WindowHandle = window;
            Info.WindowHandle = window; // синхронизируем
            }

            // Input state queries
        bool IsKeyPressed ( int key ) const;
        bool IsKeyJustPressed ( int key ) const;
        bool IsKeyReleased ( int key ) const;

        bool IsMouseButtonPressed ( int button ) const;
        bool IsMouseButtonJustPressed ( int button ) const;
        FVector2D GetMousePosition () const;
        FVector2D GetMouseDelta () const;
        FVector2D GetScrollDelta () const;

        // Mouse control
        void SetMouseCursorVisible ( bool visible );
        void SetMousePosition ( const FVector2D & position );

        // Controller input processing
        void ProcessControllerInput ( CPlayerController * Controller, float DeltaTime );

        // Input binding for components
        void BindAction ( const std::string & actionName, int button, InputActionDelegate delegate, CInputComponent * component );
        void BindAxis ( const std::string & axisName, int positiveKey, int negativeKey,
                        InputAxisDelegate delegate, CInputComponent * component = nullptr );

        void UnbindAction ( const std::string & actionName, CInputComponent * component = nullptr );
        void UnbindAxis ( const std::string & axisName, CInputComponent * component = nullptr );

        // Component registration
        void RegisterInputComponent ( CInputComponent * Component );
        void UnregisterInputComponent ( CInputComponent * Component );

        // Callback handlers (public для доступа из диспетчера)
        void HandleKey ( int key, int scancode, int action, int mods );
        void HandleMouseButton ( int button, int action, int mods );
        void HandleMouseMove ( double xpos, double ypos );
        void HandleScroll ( double xoffset, double yoffset );

    private:
        GLFWwindow * m_WindowHandle = nullptr;  // Локальная копия
        FEngineInfo & Info;  // ссылка на внешнюю структуру

        struct KeyState
            {
            bool current = false;
            bool previous = false;
            bool justPressed = false;  // Флаг для однократного нажатия
            bool justReleased = false; // Флаг для однократного отпускания
            };

        struct MouseButtonState
            {
            bool current = false;
            bool previous = false;
            bool justPressed = false;  // Флаг для однократного нажатия
            bool justReleased = false; // Флаг для однократного отпускания
            };

        struct ActionBinding
            {
            int button;
            InputActionDelegate delegate;
            CInputComponent * owner = nullptr;
            };

        struct AxisBinding
            {
            int positiveKey;
            int negativeKey;
            InputAxisDelegate delegate;
            CInputComponent * owner = nullptr;
            float value = 0.0f;
            };

        void UpdateMouseState ();
        void ProcessActions ( float DeltaTime );
        void ProcessAxes ( float DeltaTime );

    private:
        static CInputSystem * s_Instance;

        bool bIsInitialized = false;

        // Input state
        std::unordered_map<int, KeyState> m_KeyStates;
        std::unordered_map<int, MouseButtonState> m_MouseButtonStates;
        FVector2D m_MousePosition = FVector2D ( 0.0f );
        FVector2D m_LastMousePosition = FVector2D ( 0.0f );
        FVector2D m_MouseDelta = FVector2D ( 0.0f );
        FVector2D m_ScrollDelta = FVector2D ( 0.0f );

        // Input bindings
        std::unordered_map<std::string, ActionBinding> m_ActionBindings;
        std::unordered_map<std::string, AxisBinding> m_AxisBindings;

        // Registered input components
        std::vector<CInputComponent *> m_InputComponents;

        friend class CInputComponent;
    };

#define INPUT_SYSTEM CInputSystem::GetInstance()