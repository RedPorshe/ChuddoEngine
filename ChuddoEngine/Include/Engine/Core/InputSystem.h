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

// Типы событий для действий
enum class EInputEvent
    {
    IE_Pressed,     // Однократное нажатие
    IE_Released,    // Однократное отпускание
    IE_Repeat,      // Зажато (повторяется каждый кадр)
    IE_DoubleClick  // Двойной клик (если понадобится)
    };

    // Делегаты
using InputActionDelegate = std::function<void ()>;  // Для IE_Pressed/IE_Released
using InputAxisDelegate = std::function<void ( float )>;
using InputRepeatDelegate = std::function<void ()>;  // Для зажатых клавиш

// Константы для осей мыши
namespace EMouseAxis
    {
    const int MouseX = 1000;
    const int MouseY = 1001;
    const int MouseScroll = 1002;
    }

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
            Info.WindowHandle = window;
            }

            // Input state queries
        bool IsKeyPressed ( int key ) const;
        bool IsKeyJustPressed ( int key ) const;
        bool IsKeyJustReleased ( int key ) const;
        bool IsKeyHeld ( int key ) const;  // Зажата (для repeat)

        bool IsMouseButtonPressed ( int button ) const;
        bool IsMouseButtonJustPressed ( int button ) const;
        bool IsMouseButtonJustReleased ( int button ) const;
        bool IsMouseButtonHeld ( int button ) const;  // Зажата

        FVector2D GetMousePosition () const;
        FVector2D GetMouseDelta () const;
        FVector2D GetScrollDelta () const;

        // Mouse control
        void SetMouseCursorVisible ( bool visible );
        void SetMousePosition ( const FVector2D & position );
        void SetMouseSensitivity ( float Sensitivity ) { m_MouseSensitivity = Sensitivity; }
        float GetMouseSensitivity () const { return m_MouseSensitivity; }

        // Controller input processing
        void ProcessControllerInput ( CPlayerController * Controller, float DeltaTime );

        // ЕДИНЫЙ BindAction для всего!
        void BindAction ( const std::string & actionName, int button, EInputEvent eventType,
                          InputActionDelegate delegate, CInputComponent * component = nullptr );

        // BindAxis для клавиатуры
        void BindAxis ( const std::string & axisName, int positiveKey, int negativeKey,
                        InputAxisDelegate delegate, CInputComponent * component = nullptr );

        // BindMouseAxis для мыши
        void BindMouseAxis ( const std::string & axisName, int mouseAxis,
                             InputAxisDelegate delegate, CInputComponent * component = nullptr );

        void UnbindAction ( const std::string & actionName, CInputComponent * component = nullptr );
        void UnbindAxis ( const std::string & axisName, CInputComponent * component = nullptr );
        void UnbindAllForComponent ( CInputComponent * component );

        // Component registration
        void RegisterInputComponent ( CInputComponent * Component );
        void UnregisterInputComponent ( CInputComponent * Component );

        // Callback handlers (public для доступа из диспетчера)
        void HandleKey ( int key, int scancode, int action, int mods );
        void HandleMouseButton ( int button, int action, int mods );
        void HandleMouseMove ( double xpos, double ypos );
        void HandleScroll ( double xoffset, double yoffset );

    private:
        GLFWwindow * m_WindowHandle = nullptr;
        FEngineInfo & Info;

        struct KeyState
            {
            bool current = false;
            bool previous = false;
            bool justPressed = false;
            bool justReleased = false;
            float heldTime = 0.0f;  // Время удержания
            };

        struct MouseButtonState
            {
            bool current = false;
            bool previous = false;
            bool justPressed = false;
            bool justReleased = false;
            float heldTime = 0.0f;
            };

        struct ActionBinding
            {
            int button;
            EInputEvent eventType;
            InputActionDelegate delegate;
            CInputComponent * owner = nullptr;
            };

        struct AxisBinding
            {
            int positiveKey = -1;
            int negativeKey = -1;
            int mouseAxis = -1;
            bool bIsMouseAxis = false;
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
        float m_MouseSensitivity = 0.1f;

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