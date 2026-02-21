#include "Core/InputSystem.h"
#include "Components/InputComponent.h"
#include "Actors/PlayerController.h"
#include "Core/Engine.h"
#include "Actors/Pawn.h"
#include <GLFW/glfw3.h>

CInputSystem * CInputSystem::s_Instance = nullptr;

CInputSystem * CInputSystem::GetInstance ()
    {
    if (s_Instance == nullptr)
        {
        FEngineInfo Inform {};
        Inform.vkInstance = VK_NULL_HANDLE;
        Inform.WindowHandle = nullptr;
        s_Instance = new CInputSystem ( Inform );
        }
    return s_Instance;
    }

CInputSystem::CInputSystem ( FEngineInfo & info ) : Info ( info )
    {
    m_WindowHandle = info.WindowHandle;
    if (!s_Instance)
        {
        s_Instance = this;
        }
    }

CInputSystem::~CInputSystem ()
    {
    ShutdownSystem ();
    if (s_Instance == this)
        {
        s_Instance = nullptr;
        }
    }

bool CInputSystem::Initialize ( FEngineInfo & info )
    {
    m_WindowHandle = info.WindowHandle;
    LOG_DEBUG ( "Window handle in CInputSystem::Initialize: ", m_WindowHandle );
    bIsInitialized = true;
    LOG_DEBUG ( "[INPUTSYSTEM] Input system initialized successfully" );
    return true;
    }

void CInputSystem::ShutdownSystem ()
    {
    LOG_DEBUG ( "[INPUTSYSTEM] Shutting down input system" );
    m_ActionBindings.clear ();
    m_AxisBindings.clear ();
    m_KeyStates.clear ();
    m_MouseButtonStates.clear ();
    m_InputComponents.clear ();
    }

void CInputSystem::Update ( float DeltaTime )
    {
    if (!m_WindowHandle)
        {
        LOG_WARN ( "[INPUTSYSTEM] Cannot update - m_WindowHandle is null" );
        return;
        }

    if (glfwWindowShouldClose ( m_WindowHandle ))
        {
        return;
        }

    if (glfwGetKey ( m_WindowHandle, GLFW_KEY_ESCAPE ) == GLFW_PRESS)
        {
        LOG_DEBUG ( "[INPUTSYSTEM] ESC pressed, exiting..." );
        if (CEngine::Get ().IsRunning ())
            {
            CEngine::Get ().RequestExit ();
            }
        return;
        }

  
    UpdateMouseState ();
    ProcessActions ( DeltaTime );
    ProcessAxes ( DeltaTime );

    m_ScrollDelta = FVector2D ( 0.0f );
        // Сбрасываем флаги justPressed/justReleased в начале каждого кадра
    for (auto & [key, state] : m_KeyStates)
        {
        state.justPressed = false;
        state.justReleased = false;
        }

    for (auto & [button, state] : m_MouseButtonStates)
        {
        state.justPressed = false;
        state.justReleased = false;
        }
    }

GLFWwindow * CInputSystem::GetWindow () const
    {
    return m_WindowHandle;
    }

void CInputSystem::ProcessControllerInput ( CPlayerController * Controller, float DeltaTime )
    {
    if (!Controller || !Controller->IsInputEnabled ()) return;

    CPawn * ControlledPawn = Controller->GetPawn ();
    if (!ControlledPawn || !ControlledPawn->IsInputEnabled ()) return;

    CInputComponent * InputComp = ControlledPawn->GetInputComponent ();
    if (InputComp == nullptr) return;

    InputComp->Tick ( DeltaTime );
    }

bool CInputSystem::IsKeyPressed ( int key ) const
    {
    auto it = m_KeyStates.find ( key );
    return it != m_KeyStates.end () && it->second.current;
    }

bool CInputSystem::IsKeyJustPressed ( int key ) const
    {
    auto it = m_KeyStates.find ( key );
    return it != m_KeyStates.end () && it->second.justPressed;
    }

bool CInputSystem::IsKeyReleased ( int key ) const
    {
    auto it = m_KeyStates.find ( key );
    return it != m_KeyStates.end () && it->second.justReleased;
    }

bool CInputSystem::IsMouseButtonPressed ( int button ) const
    {
    auto it = m_MouseButtonStates.find ( button );
    return it != m_MouseButtonStates.end () && it->second.current;
    }

bool CInputSystem::IsMouseButtonJustPressed ( int button ) const
    {
    auto it = m_MouseButtonStates.find ( button );
    return it != m_MouseButtonStates.end () && it->second.justPressed;
    }

FVector2D CInputSystem::GetMousePosition () const
    {
    return m_MousePosition;
    }

FVector2D CInputSystem::GetMouseDelta () const
    {
    return m_MouseDelta;
    }

FVector2D CInputSystem::GetScrollDelta () const
    {
    return m_ScrollDelta;
    }

void CInputSystem::SetMouseCursorVisible ( bool visible )
    {
    if (m_WindowHandle)
        {
        glfwSetInputMode ( m_WindowHandle, GLFW_CURSOR,
                           visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED );
        }
    }

void CInputSystem::SetMousePosition ( const FVector2D & position )
    {
    if (m_WindowHandle)
        {
        glfwSetCursorPos ( m_WindowHandle, position.x, position.y );
        }
    }

    // ЕДИНЫЙ BindAction для всего!
void CInputSystem::BindAction ( const std::string & actionName, int button, InputActionDelegate delegate, CInputComponent * component )
    {
    ActionBinding binding;
    binding.button = button;
    binding.delegate = delegate;
    binding.owner = component;
    m_ActionBindings[ actionName ] = binding;

    // Определяем тип по значению для отладки
    const char * typeStr = ( button >= GLFW_MOUSE_BUTTON_1 && button <= GLFW_MOUSE_BUTTON_LAST ) ? "mouse button" : "key";
    LOG_DEBUG ( "[INPUTSYSTEM] Bound action: ", actionName, " to ", typeStr, ": ", button );
    }

void CInputSystem::BindAxis ( const std::string & axisName, int positiveKey, int negativeKey, InputAxisDelegate delegate, CInputComponent * component )
    {
    AxisBinding binding;
    binding.positiveKey = positiveKey;
    binding.negativeKey = negativeKey;
    binding.delegate = delegate;
    binding.owner = component;
    binding.value = 0.0f;
    m_AxisBindings[ axisName ] = binding;

    LOG_DEBUG ( "[INPUTSYSTEM] Bound axis: ", axisName, " to keys: ", positiveKey, " / ", negativeKey );
    }

void CInputSystem::UnbindAction ( const std::string & actionName, CInputComponent * component )
    {
    auto it = m_ActionBindings.find ( actionName );
    if (it != m_ActionBindings.end () && ( !component || it->second.owner == component ))
        {
        m_ActionBindings.erase ( it );
        LOG_DEBUG ( "[INPUTSYSTEM] Unbound action: ", actionName );
        }
    }

void CInputSystem::UnbindAxis ( const std::string & axisName, CInputComponent * component )
    {
    auto it = m_AxisBindings.find ( axisName );
    if (it != m_AxisBindings.end () && ( !component || it->second.owner == component ))
        {
        m_AxisBindings.erase ( it );
        LOG_DEBUG ( "[INPUTSYSTEM] Unbound axis: ", axisName );
        }
    }

void CInputSystem::RegisterInputComponent ( CInputComponent * Component )
    {
    if (Component && std::find ( m_InputComponents.begin (), m_InputComponents.end (), Component ) == m_InputComponents.end ())
        {
        m_InputComponents.push_back ( Component );
        LOG_DEBUG ( "[INPUTSYSTEM] Registered input component: ", Component->GetName () );
        }
    }

void CInputSystem::UnregisterInputComponent ( CInputComponent * Component )
    {
    auto it = std::find ( m_InputComponents.begin (), m_InputComponents.end (), Component );
    if (it != m_InputComponents.end ())
        {
        m_InputComponents.erase ( it );
        LOG_DEBUG ( "[INPUTSYSTEM] Unregistered input component: ", Component->GetName () );
        }
    }

void CInputSystem::UpdateMouseState ()
    {
        // Обновляем только current/previous, но НЕ justPressed/justReleased
    for (auto & [button, state] : m_MouseButtonStates)
        {
        state.previous = state.current;
        state.current = glfwGetMouseButton ( m_WindowHandle, button ) == GLFW_PRESS;
        }

    m_MouseDelta = m_MousePosition - m_LastMousePosition;
    m_LastMousePosition = m_MousePosition;
    }

void CInputSystem::ProcessActions ( float DeltaTime )
    {
    for (const auto & [actionName, binding] : m_ActionBindings)
        {
        bool triggered = false;

        // Проверяем - это кнопка мыши или клавиатуры?
        if (binding.button >= GLFW_MOUSE_BUTTON_1 && binding.button <= GLFW_MOUSE_BUTTON_LAST)
            {
                // Это кнопка мыши
            triggered = IsMouseButtonJustPressed ( binding.button );
            }
        else
            {
                // Это клавиша клавиатуры
            triggered = IsKeyJustPressed ( binding.button );
            }

        if (triggered)
            {
           

            if (binding.delegate)
                {
                binding.delegate ( 1.f );                
                }
            else
                {
                LOG_ERROR ( "[INPUTSYSTEM] >>> Delegate is null for action: ", actionName );
                }
            }
        }
    }

void CInputSystem::ProcessAxes ( float DeltaTime )
    {
    for (auto & [axisName, binding] : m_AxisBindings)
        {
        float value = 0.0f;

        if (IsKeyPressed ( binding.positiveKey )) value += 1.0f;
        if (IsKeyPressed ( binding.negativeKey )) value -= 1.0f;

       
        float previousValue = binding.value;
        binding.value = value;

        if (value != previousValue || value != 0.0f)
            {
            binding.delegate ( value );
            }
        }
    }

void CInputSystem::HandleKey ( int key, int scancode, int action, int mods )
    {
    if (action == GLFW_PRESS)
        {
        m_KeyStates[ key ].previous = m_KeyStates[ key ].current;
        m_KeyStates[ key ].current = true;
        m_KeyStates[ key ].justPressed = true; 
        m_KeyStates[ key ].justReleased = false;        
        }
    else if (action == GLFW_RELEASE)
        {        
        m_KeyStates[ key ].previous = m_KeyStates[ key ].current;
        m_KeyStates[ key ].current = false;
        m_KeyStates[ key ].justPressed = false;
        m_KeyStates[ key ].justReleased = true;
        }
        // GLFW_REPEAT игнорируем для состояний
    }

void CInputSystem::HandleMouseButton ( int button, int action, int mods )
    {
    if (action == GLFW_PRESS)
        {
        m_MouseButtonStates[ button ].previous = m_MouseButtonStates[ button ].current;
        m_MouseButtonStates[ button ].current = true;
        m_MouseButtonStates[ button ].justPressed = true;
        m_MouseButtonStates[ button ].justReleased = false;
       
        }
    else if (action == GLFW_RELEASE)
        {
        m_MouseButtonStates[ button ].previous = m_MouseButtonStates[ button ].current;
        m_MouseButtonStates[ button ].current = false;
        m_MouseButtonStates[ button ].justPressed = false;
        m_MouseButtonStates[ button ].justReleased = true;
        }
    }

void CInputSystem::HandleMouseMove ( double xpos, double ypos )
    {
    m_MousePosition = FVector2D ( static_cast< float >( xpos ), static_cast< float >( ypos ) );
    }

void CInputSystem::HandleScroll ( double xoffset, double yoffset )
    {
    m_ScrollDelta = FVector2D ( static_cast< float >( xoffset ), static_cast< float >( yoffset ) );
    }