#pragma once

#include "Components/BaseComponent.h"
#include <functional>
#include <glm/glm.hpp>
#include "Core/InputSystem.h"  // Для EInputEvent

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
        virtual void OnEndPlay () ;

        // Input query methods
        bool IsKeyPressed ( int key ) const;
        bool IsKeyJustPressed ( int key ) const;
        bool IsKeyJustReleased ( int key ) const;
        bool IsKeyHeld ( int key ) const;

        bool IsMouseButtonPressed ( int button ) const;
        bool IsMouseButtonJustPressed ( int button ) const;
        bool IsMouseButtonJustReleased ( int button ) const;
        bool IsMouseButtonHeld ( int button ) const;

        FVector2D GetMousePosition () const;
        FVector2D GetMouseDelta () const;
        FVector2D GetScrollDelta () const;

        // ЕДИНЫЙ BindAction для всего!
        void BindAction ( const std::string & actionName, int button, EInputEvent eventType,
                          std::function<void ()> callback );

        // BindAxis для клавиатуры
        void BindAxis ( const std::string & axisName, int positiveKey, int negativeKey,
                        std::function<void ( float )> callback );

        // BindMouseAxis для мыши
        void BindMouseAxisX ( const std::string & axisName, std::function<void ( float )> callback );
        void BindMouseAxisY ( const std::string & axisName, std::function<void ( float )> callback );
        void BindMouseScroll ( const std::string & axisName, std::function<void ( float )> callback );

        // Утилиты для биндинга с чувствительностью
        void BindMouseAxisXWithSensitivity ( const std::string & axisName,
                                             std::function<void ( float )> callback,
                                             float sensitivity = 0.1f );
        void BindMouseAxisYWithSensitivity ( const std::string & axisName,
                                             std::function<void ( float )> callback,
                                             float sensitivity = 0.1f );

        void UnbindAction ( const std::string & actionName );
        void UnbindAxis ( const std::string & axisName );
        void UnbindAll ();

        // Mouse control
        void SetMouseCursorVisible ( bool visible );
        void SetMousePosition ( const FVector2D & position );
        void SetMouseSensitivity ( float sensitivity );

        // Getters
        CPawn * GetOwningPawn () const;
        CInputSystem * GetInputSystem () const;

        // Debug
        void PrintBoundActions () const;
        void PrintBoundAxes () const;

    protected:
        std::vector<std::string> m_BoundActions;
        std::vector<std::string> m_BoundAxes;
    };

REGISTER_CLASS_FACTORY ( CInputComponent );