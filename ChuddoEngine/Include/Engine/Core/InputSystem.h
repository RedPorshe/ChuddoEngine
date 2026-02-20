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

// Input action delegate
using InputActionDelegate = std::function<void ( float )>;
using InputAxisDelegate = std::function<void ( float )>;

class CInputSystem 
    {
    public:
        // Singleton access
        static CInputSystem * GetInstance ();

        CInputSystem ( );
        virtual ~CInputSystem ();

        // System overrides
         bool Initialize ( GLFWwindow * window ) ;
         void ShutdownSystem () ;
         void Update ( float DeltaTime ) ;
         bool IsInitialized () const { return bIsInitialized; }
        // GLFW window association
        void SetWindow ( GLFWwindow * window );
        GLFWwindow * GetWindow () const { return m_pWindow; }

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
        void BindAction ( const std::string & actionName, int key, InputActionDelegate delegate, CInputComponent * component = nullptr );
        void BindAxis ( const std::string & axisName, int positiveKey, int negativeKey, InputAxisDelegate delegate, CInputComponent * component = nullptr );

        void UnbindAction ( const std::string & actionName, CInputComponent * component = nullptr );
        void UnbindAxis ( const std::string & axisName, CInputComponent * component = nullptr );

        // Component registration
        void RegisterInputComponent ( CInputComponent * Component );
        void UnregisterInputComponent ( CInputComponent * Component );

    private:
        struct KeyState
            {
            bool current = false;
            bool previous = false;
            };

        struct MouseButtonState
            {
            bool current = false;
            bool previous = false;
            };

        struct ActionBinding
            {
            int key;
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

            // Static GLFW callbacks
        static void KeyCallback ( GLFWwindow * window, int key, int scancode, int action, int mods );
        static void MouseButtonCallback ( GLFWwindow * window, int button, int action, int mods );
        static void CursorPositionCallback ( GLFWwindow * window, double xpos, double ypos );
        static void ScrollCallback ( GLFWwindow * window, double xoffset, double yoffset );

        void UpdateKeyStates ();
        void UpdateMouseState ();
        void ProcessActions ( float DeltaTime );
        void ProcessAxes ( float DeltaTime );

    private:
        static CInputSystem * s_Instance;

        GLFWwindow * m_pWindow = nullptr;
        bool bIsInitialized = false;
        // Input state
        std::unordered_map<int, KeyState> m_KeyStates;
        std::unordered_map<int, MouseButtonState> m_MouseButtonStates;
        FVector2D m_MousePosition = FVector2D ( 0.0f );
        FVector2D m_LastMousePosition = FVector2D ( 0.0f );
        FVector2D m_MouseDelta = FVector2D ( 0.0f );
        FVector2D m_ScrollDelta = FVector2D ( 0.0f );

        // Input bindings (global and per-component)
        std::unordered_map<std::string, ActionBinding> m_ActionBindings;
        std::unordered_map<std::string, AxisBinding> m_AxisBindings;

        // Registered input components
        std::vector<CInputComponent *> m_InputComponents;

        friend class CInputComponent;
    };

#define INPUT_SYSTEM CInputSystem::GetInstance ()