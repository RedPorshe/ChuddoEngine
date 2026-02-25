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
        bool IsGrounded () const { return bIsOnGround; }
        float GetVerticalVelocity () const { return m_VerticalVelocity; }
        void SetVerticalVelocity ( float vel ) { m_VerticalVelocity = vel; }

      

    protected:
        float m_GravityScale = 1.1f;        
        float m_GravityStrength = 9.8f;      
        float m_VerticalVelocity = 0.0f;      
        float m_KillZone = -1000.0f;      // Killzone
        float m_GroundCheckDistance = 1.5f;    
        bool bIsOnGround = false;           
        bool m_bWasGrounded = false;           
        void CheckGrounded ();
        FVector ResolveTerrainCollision ( const FVector & desiredPos, const FVector & currentPos );
        FVector m_LastPosition;

    };

REGISTER_CLASS_FACTORY ( CGravityComponent );