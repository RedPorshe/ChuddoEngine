#pragma once
#include "Components/BaseComponent.h"

class CGravityComponent : public CBaseComponent
    {
    CHUDDO_DECLARE_CLASS ( CGravityComponent, CBaseComponent );

    public:
        CGravityComponent ( CObject * inOwner = nullptr, const std::string & inDisplayName = "GravityComponent" );
        virtual ~CGravityComponent ();

        virtual void InitComponent () override;
        virtual void Tick ( float DeltaTime ) override;
        virtual void OnBeginPlay () override;

        // Настройки гравитации
        void SetGravityScale ( float scale ) { m_GravityScale = scale; }
        float GetGravityScale () const { return m_GravityScale; }

        void SetGravityStrength ( float strength ) { m_GravityStrength = strength; }
        float GetGravityStrength () const { return m_GravityStrength; }

        // Состояние
        bool IsGrounded () const { return bIsOnGround; }
        float GetVerticalVelocity () const { return m_VerticalVelocity; }
        void SetVerticalVelocity ( float vel ) { m_VerticalVelocity = vel; }

    protected:
        // Параметры гравитации
        float m_GravityScale = 1.1f;
        float m_GravityStrength = 9.8f;
        float m_VerticalVelocity = 0.0f;
        float m_KillZone = -10000.0f;
        float m_MaxFallSpeed = -100.0f;

        // Состояние
        bool bIsOnGround = false;
        bool m_bWasGrounded = false;

        // Для коллизий
        FVector m_LastPosition = FVector::Zero ();
    };

REGISTER_CLASS_FACTORY ( CGravityComponent );