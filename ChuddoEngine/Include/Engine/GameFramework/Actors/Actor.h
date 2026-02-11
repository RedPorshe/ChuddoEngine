#pragma once

#include "Core/Object.h"

class CWorld;
class CLevel;
class CBaseComponent;
class CTransformComponent;
class CBaseCollisionComponent;

class CActor : public CObject
    {
    CHUDDO_DECLARE_CLASS ( CActor, CObject )

    public:
        CActor ( CObject * owner = nullptr, const std::string & inName = "Actor" );
        virtual ~CActor ();

        virtual void BeginPlay ();
        virtual void Tick ( float deltaTime );
        virtual void EndPlay ();

        bool IsMIsMoving () { return bIsMovin; }

        // Getters
        CTransformComponent * GetRootComponent () const { return RootComponent; }
        CLevel * GetLevel () const;
        CWorld * GetWorld () const;

        bool IsCanTickOnAttached () const { return bIsCanTickAsAttached; }
        bool IsAttached () const { return bIsAttached; }
        bool IsPendingToDestroy () const { return bIsPendingToDestroy; }
        bool IsLerpingLocation () const { return bIsLerpingLocation; }
        bool IsLerpingRotation () const { return bIsLerpingRotation; }

        // Setters
        void SetRootComponent ( CTransformComponent * NewRoot );
        void SetCanTickOnAttached ( bool value ) { bIsCanTickAsAttached = value; }
        void SetIsAttached ( bool value ) { bIsAttached = value; }
        void SetActorName ( const std::string & newName );

        // Actor lifecycle
        void Destroy ();
        void SetPendingToDestroy ();

        // Component management
        template<typename Comp, typename... Args>
        Comp * AddDefaultSubObject ( const std::string & desiredDisplayName = "SubObject" );

        CBaseComponent * AddDefaultSubObject ( const std::string & className,
                                               const std::string & desiredDisplayName );

          // Transform getters
        FVector GetActorLocation ();
        FVector GetActorRotation ();
        FVector GetActorScale ();
        FQuat GetActorRotationQuat ();

        // Direction vectors getters
        FVector GetActorForwardVector ();
        FVector GetActorRightVector ();
        FVector GetActorUpVector ();

        // Transform setters
        void SetActorLocation ( const FVector & InLocation );
        void SetActorLocation ( float inX, float inY, float inZ );
        void SetActorScale ( const FVector & InScale );
        void SetActorScale ( float inX, float inY, float inZ );
        void SetActorScale ( float InScale );
        void SetActorRotation ( const FVector & inRotation );
        void SetActorRotation ( const FQuat & inRotation );
        void SetActorRotation ( float inX, float inY, float inZ );

        // Movement methods
        void MoveActor ( const FVector & Delta, bool Interpolate = true );
        void RotateActor ( const FVector & DeltaRotation, bool Interpolate = true );
        void RotateActor ( const FQuat & DeltaRotation, bool Interpolate = true ); // НОВЫЙ: Перегрузка для кватерниона

        // Offset methods (world and local space)
        void AddActorWorldOffset ( const FVector & DeltaLocation, bool Interpolate = false );
        void AddActorLocalOffset ( const FVector & DeltaLocation, bool Interpolate = false );
        void AddActorWorldRotation ( const FQuat & DeltaRotation, bool Interpolate = false );
        void AddActorLocalRotation ( const FQuat & DeltaRotation, bool Interpolate = false );

        // Helper movement methods
        void MoveActorInDirection ( const FVector & Direction, float Distance, bool Interpolate = true );
        void RotateAroundAxis ( const FVector & Axis, float AngleDegrees, bool Interpolate = true );
        void SetInterpolationSpeed ( const float inSpeed ) { LerpSpeed = inSpeed; }
        std::vector<CBaseComponent *> GetActorComponents () const { return ActorComponents; }

        // Teleport functions (immediate movement)
        void TeleportTo ( const FVector & NewLocation );
        void TeleportTo ( float NewX, float NewY, float NewZ );
        void SetActorRotationImmediately ( const FQuat & NewRotation );
        void SetActorRotationImmediately ( const FVector & NewRotation );
        void SetActorRotationImmediately ( float inX, float inY, float inZ );

        void SetCollisionEnabled ( bool value = true );

        void OnComponentBeginOverlap ( CBaseCollisionComponent * other );
        void OnComponentEndOverlap ( CBaseCollisionComponent * other );
        
    protected:
        std::vector<CBaseComponent *> ActorComponents;
        CTransformComponent * RootComponent = nullptr;

        // State flags
        bool bIsCanTickAsAttached { false };
        bool bIsAttached { false };
        bool bIsPendingToDestroy { false };
        bool bIsCollisionEnabled { false };
        bool bIsMovin { false };
      
        // Interpolation data
        FVector TargetLocation;
        FQuat TargetRotation;
        FVector LerpStartLocation;
        FQuat LerpStartRotation;

       
        float LocationLerpAlpha = 0.0f;
        float RotationLerpAlpha = 0.0f;

        bool bIsLerpingLocation = false;
        bool bIsLerpingRotation = false;
        float LerpSpeed = 10.0f;
    };

    // Inline template implementation
#include "Components/SceneComponent.h"

template<typename Comp, typename... Args>
inline Comp * CActor::AddDefaultSubObject ( const std::string & desiredDisplayName )
    {
    static_assert( std::is_base_of<CBaseComponent, Comp>::value,
                   "Class must be derived from CBaseComponent" );

    auto newComp = this->AddSubObject<Comp> ( desiredDisplayName );

    // If no root component exists and this is a scene component, set it as root
    if (RootComponent == nullptr)
        {
        if (CTransformComponent * sceneComp = dynamic_cast< CTransformComponent * >( newComp ))
            {
            RootComponent = sceneComp;
            }
        }

    ActorComponents.push_back ( newComp );
    return newComp;
    }

REGISTER_CLASS_FACTORY ( CActor );