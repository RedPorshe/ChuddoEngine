#pragma once

#include "Components/BaseComponent.h"
#include <functional>
#include <glm/glm.hpp>

class CPawn;
class CInputSystem;

class CInputComponent : public CBaseComponent
    {
    CHUDDO_DECLARE_CLASS ( CInputComponent, CBaseComponent );

    public:
        CInputComponent ( CObject * inOwner = nullptr, const std::string & inDisplayName = "InputComponent" );
        virtual ~CInputComponent ();

        virtual void InitComponent () override;
        virtual void Tick ( float DeltaTime ) override;
        virtual void OnBeginPlay () override;
        virtual void onEndPlay ();

        // Input query methods
        bool IsKeyPressed ( int key ) const;
        bool IsKeyJustPressed ( int key ) const;
        bool IsMouseButtonPressed ( int button ) const;
        FVector2D GetMousePosition () const;
        FVector2D GetMouseDelta () const;
        FVector2D GetScrollDelta () const;

        // ЕДИНЫЙ BindAction для всего!
        void BindAction ( const std::string & actionName, int button, std::function<void ( float )> callback );
        void BindAxis ( const std::string & axisName, int positiveKey, int negativeKey, std::function<void ( float )> callback );

        void UnbindAction ( const std::string & actionName );
        void UnbindAxis ( const std::string & axisName );
        void UnbindAll ();

        // Mouse control
        void SetMouseCursorVisible ( bool visible );
        void SetMousePosition ( const FVector2D & position );

        // Getters
        CPawn * GetOwningPawn () const;
        CInputSystem * GetInputSystem () const;

    protected:
        std::vector<std::string> m_BoundActions;
        std::vector<std::string> m_BoundAxes;
    };

REGISTER_CLASS_FACTORY ( CInputComponent );