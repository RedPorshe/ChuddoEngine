#include "Core/InputSystem.h"
#include "Components/InputComponent.h"
#include "Actors/PlayerController.h"
#include "Actors/Pawn.h"
#include <GLFW/glfw3.h>

// Initialize singleton instance
CInputSystem * CInputSystem::s_Instance = nullptr;

CInputSystem * CInputSystem::GetInstance ()
    {
    if (s_Instance == nullptr)
        {
        s_Instance = new CInputSystem ();
        }
    return s_Instance;
    }

CInputSystem::CInputSystem ( )   
    {
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

bool CInputSystem::Initialize ( GLFWwindow * window )
    {
    if (window == nullptr)
        {
        LOG_ERROR ( "[INPUTSYSTEM] Cannot initialize input system without a valid GLFW window!" );
        return false;
        }
    m_pWindow = window;

    LOG_DEBUG ( "[INPUTSYSTEM] Initializing input system" );

    if (!m_pWindow)
        {
        LOG_ERROR ( "[INPUTSYSTEM] No window associated with input system!" );
        return false;
        }

        // Set GLFW callbacks
    glfwSetKeyCallback ( m_pWindow, KeyCallback );
    glfwSetMouseButtonCallback ( m_pWindow, MouseButtonCallback );
    glfwSetCursorPosCallback ( m_pWindow, CursorPositionCallback );
    glfwSetScrollCallback ( m_pWindow, ScrollCallback );

    // Store this instance in GLFW user pointer for callbacks
    glfwSetWindowUserPointer ( m_pWindow, this );
    bIsInitialized = true;
    LOG_DEBUG ( "[INPUTSYSTEM] Input system initialized successfully" );
    return true;
    }

void CInputSystem::ShutdownSystem ()
    {
    LOG_DEBUG ( "[INPUTSYSTEM] Shutting down input system" );

    // Clear all bindings
    m_ActionBindings.clear ();
    m_AxisBindings.clear ();
    m_KeyStates.clear ();
    m_MouseButtonStates.clear ();
    m_InputComponents.clear ();

    if (m_pWindow)
        {
            // Reset GLFW callbacks
        glfwSetKeyCallback ( m_pWindow, nullptr );
        glfwSetMouseButtonCallback ( m_pWindow, nullptr );
        glfwSetCursorPosCallback ( m_pWindow, nullptr );
        glfwSetScrollCallback ( m_pWindow, nullptr );
        glfwSetWindowUserPointer ( m_pWindow, nullptr );
        }

    
    }

void CInputSystem::Update ( float DeltaTime )
    {   
    if (!m_pWindow)
        return;

    // Update input states
    UpdateKeyStates ();
    UpdateMouseState ();

    // Process global bindings
    ProcessActions ( DeltaTime );
    ProcessAxes ( DeltaTime );

    // Reset scroll delta each frame
    m_ScrollDelta = FVector2D ( 0.0f );
    }

void CInputSystem::SetWindow ( GLFWwindow * window )
    {
    m_pWindow = window;
    if (m_pWindow && IsInitialized ())
        {
            // Re-setup callbacks if system is already initialized
        glfwSetKeyCallback ( m_pWindow, KeyCallback );
        glfwSetMouseButtonCallback ( m_pWindow, MouseButtonCallback );
        glfwSetCursorPosCallback ( m_pWindow, CursorPositionCallback );
        glfwSetScrollCallback ( m_pWindow, ScrollCallback );
        glfwSetWindowUserPointer ( m_pWindow, this );
        }
    }

void CInputSystem::ProcessControllerInput ( CPlayerController * Controller, float DeltaTime )
    {
    if (!Controller || !Controller->IsInputEnabled ())
        return;

    CPawn * ControlledPawn = Controller->GetPawn ();
    if (!ControlledPawn || !ControlledPawn->IsInputEnabled ())
        return;

    CInputComponent * InputComp = ControlledPawn->GetInputComponent ();
    if (!InputComp )
        return;

    // Let the input component process input
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
    return it != m_KeyStates.end () && it->second.current && !it->second.previous;
    }

bool CInputSystem::IsKeyReleased ( int key ) const
    {
    auto it = m_KeyStates.find ( key );
    return it != m_KeyStates.end () && !it->second.current && it->second.previous;
    }

bool CInputSystem::IsMouseButtonPressed ( int button ) const
    {
    auto it = m_MouseButtonStates.find ( button );
    return it != m_MouseButtonStates.end () && it->second.current;
    }

bool CInputSystem::IsMouseButtonJustPressed ( int button ) const
    {
    auto it = m_MouseButtonStates.find ( button );
    return it != m_MouseButtonStates.end () && it->second.current && !it->second.previous;
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
    if (m_pWindow)
        {
        glfwSetInputMode ( m_pWindow, GLFW_CURSOR,
                           visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED );
        }
    }

void CInputSystem::SetMousePosition ( const FVector2D & position )
    {
    if (m_pWindow)
        {
        glfwSetCursorPos ( m_pWindow, position.x, position.y );
        }
    }

void CInputSystem::BindAction ( const std::string & actionName, int key, InputActionDelegate delegate, CInputComponent * component )
    {
    ActionBinding binding;
    binding.key = key;
    binding.delegate = delegate;
    binding.owner = component;
    m_ActionBindings[ actionName ] = binding;

    LOG_DEBUG ( "[INPUTSYSTEM] Bound action: ", actionName, " to key: ", key );
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

void CInputSystem::UpdateKeyStates ()
    {
    for (auto & [key, state] : m_KeyStates)
        {
        state.previous = state.current;
        state.current = glfwGetKey ( m_pWindow, key ) == GLFW_PRESS;
        }
    }

void CInputSystem::UpdateMouseState ()
    {
        // Update mouse button states
    for (auto & [button, state] : m_MouseButtonStates)
        {
        state.previous = state.current;
        state.current = glfwGetMouseButton ( m_pWindow, button ) == GLFW_PRESS;
        }

        // Calculate mouse delta
    m_MouseDelta = m_MousePosition - m_LastMousePosition;
    m_LastMousePosition = m_MousePosition;
    }

void CInputSystem::ProcessActions ( float DeltaTime )
    {
    for (const auto & [actionName, binding] : m_ActionBindings)
        {
        if (IsKeyJustPressed ( binding.key ))
            {
            binding.delegate ( DeltaTime );
            }
        }
    }

void CInputSystem::ProcessAxes ( float DeltaTime )
    {
    for (auto & [axisName, binding] : m_AxisBindings)
        {
        float value = 0.0f;

        if (IsKeyPressed ( binding.positiveKey ))
            value += 1.0f;
        if (IsKeyPressed ( binding.negativeKey ))
            value -= 1.0f;

        if (value != binding.value)
            {
            binding.value = value;
            binding.delegate ( value );
            }
        }
    }

    // Static GLFW callbacks
void CInputSystem::KeyCallback ( GLFWwindow * window, int key, int scancode, int action, int mods )
    {
    CInputSystem * system = static_cast< CInputSystem * >( glfwGetWindowUserPointer ( window ) );
    if (!system)
        return;

    if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
        bool pressed = ( action == GLFW_PRESS );
        system->m_KeyStates[ key ].previous = system->m_KeyStates[ key ].current;
        system->m_KeyStates[ key ].current = pressed;
        }
    }

void CInputSystem::MouseButtonCallback ( GLFWwindow * window, int button, int action, int mods )
    {
    CInputSystem * system = static_cast< CInputSystem * >( glfwGetWindowUserPointer ( window ) );
    if (!system)
        return;

    if (action == GLFW_PRESS || action == GLFW_RELEASE)
        {
        bool pressed = ( action == GLFW_PRESS );
        system->m_MouseButtonStates[ button ].previous = system->m_MouseButtonStates[ button ].current;
        system->m_MouseButtonStates[ button ].current = pressed;
        }
    }

void CInputSystem::CursorPositionCallback ( GLFWwindow * window, double xpos, double ypos )
    {
    CInputSystem * system = static_cast< CInputSystem * >( glfwGetWindowUserPointer ( window ) );
    if (!system)
        return;

    system->m_MousePosition = FVector2D ( static_cast< float >( xpos ), static_cast< float >( ypos ) );
    }

void CInputSystem::ScrollCallback ( GLFWwindow * window, double xoffset, double yoffset )
    {
    CInputSystem * system = static_cast< CInputSystem * >( glfwGetWindowUserPointer ( window ) );
    if (!system)
        return;

    system->m_ScrollDelta = FVector2D ( static_cast< float >( xoffset ), static_cast< float >( yoffset ) );
    }