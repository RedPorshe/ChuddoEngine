#include "Components/InputComponent.h"
#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "Core/InputSystem.h"

CInputComponent::CInputComponent ( CObject * inOwner, const std::string & inDisplayName )
    : Super ( inOwner, inDisplayName )
    {}

CInputComponent::~CInputComponent ()
    {
    UnbindAll ();
    }

void CInputComponent::InitComponent ()
    {
    Super::InitComponent ();

    auto * inputSystem = GetInputSystem ();
    if (inputSystem)
        {
        inputSystem->RegisterInputComponent ( this );
        }
    }

void CInputComponent::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    // Component-specific input processing can go here
    }

void CInputComponent::OnBeginPlay ()
    {
    Super::OnBeginPlay ();
    }

void CInputComponent::onEndPlay ()
    {
    UnbindAll ();    
    }

bool CInputComponent::IsKeyPressed ( int key ) const
    {
    auto * inputSystem = GetInputSystem ();
    return inputSystem ? inputSystem->IsKeyPressed ( key ) : false;
    }

bool CInputComponent::IsKeyJustPressed ( int key ) const
    {
    auto * inputSystem = GetInputSystem ();
    return inputSystem ? inputSystem->IsKeyJustPressed ( key ) : false;
    }

bool CInputComponent::IsMouseButtonPressed ( int button ) const
    {
    auto * inputSystem = GetInputSystem ();
    return inputSystem ? inputSystem->IsMouseButtonPressed ( button ) : false;
    }

FVector2D CInputComponent::GetMousePosition () const
    {
    auto * inputSystem = GetInputSystem ();
    return inputSystem ? inputSystem->GetMousePosition () : FVector2D ( 0.0f );
    }

FVector2D CInputComponent::GetMouseDelta () const
    {
    auto * inputSystem = GetInputSystem ();
    return inputSystem ? inputSystem->GetMouseDelta () : FVector2D ( 0.0f );
    }

FVector2D CInputComponent::GetScrollDelta () const
    {
    auto * inputSystem = GetInputSystem ();
    return inputSystem ? inputSystem->GetScrollDelta () : FVector2D ( 0.0f );
    }

void CInputComponent::BindAction ( const std::string & actionName, int key, std::function<void ( float )> callback )
    {
    auto * inputSystem = GetInputSystem ();
    if (inputSystem)
        {
        inputSystem->BindAction ( actionName, key, callback, this );
        m_BoundActions.push_back ( actionName );
        }
    }

void CInputComponent::BindAxis ( const std::string & axisName, int positiveKey, int negativeKey, std::function<void ( float )> callback )
    {
    auto * inputSystem = GetInputSystem ();
    if (inputSystem)
        {
        inputSystem->BindAxis ( axisName, positiveKey, negativeKey, callback, this );
        m_BoundAxes.push_back ( axisName );
        }
    }

void CInputComponent::UnbindAction ( const std::string & actionName )
    {
    auto * inputSystem = GetInputSystem ();
    if (inputSystem)
        {
        inputSystem->UnbindAction ( actionName, this );

        auto it = std::find ( m_BoundActions.begin (), m_BoundActions.end (), actionName );
        if (it != m_BoundActions.end ())
            {
            m_BoundActions.erase ( it );
            }
        }
    }

void CInputComponent::UnbindAxis ( const std::string & axisName )
    {
    auto * inputSystem = GetInputSystem ();
    if (inputSystem)
        {
        inputSystem->UnbindAxis ( axisName, this );

        auto it = std::find ( m_BoundAxes.begin (), m_BoundAxes.end (), axisName );
        if (it != m_BoundAxes.end ())
            {
            m_BoundAxes.erase ( it );
            }
        }
    }

void CInputComponent::UnbindAll ()
    {
    auto * inputSystem = GetInputSystem ();
    if (inputSystem)
        {
        for (const auto & action : m_BoundActions)
            {
            inputSystem->UnbindAction ( action, this );
            }

        for (const auto & axis : m_BoundAxes)
            {
            inputSystem->UnbindAxis ( axis, this );
            }
        }

    m_BoundActions.clear ();
    m_BoundAxes.clear ();
    }

void CInputComponent::SetMouseCursorVisible ( bool visible )
    {
    auto * inputSystem = GetInputSystem ();
    if (inputSystem)
        {
        inputSystem->SetMouseCursorVisible ( visible );
        }
    }

void CInputComponent::SetMousePosition ( const FVector2D & position )
    {
    auto * inputSystem = GetInputSystem ();
    if (inputSystem)
        {
        inputSystem->SetMousePosition ( position );
        }
    }

CPawn * CInputComponent::GetOwningPawn () const
    {
    return dynamic_cast< CPawn * >( GetOwner () );
    }

CInputSystem * CInputComponent::GetInputSystem () const
    {
    return CInputSystem::GetInstance ();
    }