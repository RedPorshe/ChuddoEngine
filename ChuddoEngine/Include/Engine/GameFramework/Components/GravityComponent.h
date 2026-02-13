// GravityComponent.h
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
        bool IsGrounded () const { return m_bIsGrounded; }
        float GetVerticalVelocity () const { return m_VerticalVelocity; }
        void SetVerticalVelocity ( float vel ) { m_VerticalVelocity = vel; }

        // Прыжок
        void Jump ( float jumpForce );

    protected:
        float m_GravityScale = 1.0f;        // Множитель гравитации
        float m_GravityStrength = 9.8f;      // Сила гравитации (м/с²)
        float m_VerticalVelocity = 0.0f;      // Вертикальная скорость
        float m_KillZone = -10000.0f;      // Killzone
        float m_GroundCheckDistance = 1.5f;    // Дистанция проверки земли
        bool m_bIsGrounded = false;            // На земле ли?
        bool m_bWasGrounded = false;           // Был на земле в прошлом кадре
        void CheckGrounded ();
        FVector ResolveTerrainCollision ( const FVector & desiredPos, const FVector & currentPos );
        FVector m_LastPosition;

    };

REGISTER_CLASS_FACTORY ( CGravityComponent );