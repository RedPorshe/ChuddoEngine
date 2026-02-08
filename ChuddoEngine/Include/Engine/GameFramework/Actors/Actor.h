#pragma once

#include "Core/Object.h"

class CWorld;
class CLevel;
class CBaseComponent;
class CTransformComponent;

class CActor : public CObject
    {
    CHUDDO_DECLARE_CLASS ( CActor, CObject )

    public:
        CActor ( CObject * owner = nullptr, const std::string & inName = "Actor" );
        virtual ~CActor ();

        virtual void BeginPlay ();
        virtual void Tick ( float deltaTime );
        virtual void EndPlay ();

        // Getters
        CTransformComponent * GetRootComponent () const { return RootComponent; }
        CLevel * GetLevel () const;
        CWorld * GetWorld () const;
       

        bool IsCanTickOnAttached () const { return bIsCanTickAsAttached; }
        bool IsAttached () const { return bIsAttached; }
        bool IsPendingToDestroy () const { return bIsPendingToDestroy; }

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


        FVector  GetActorLocation () ;
        FVector  GetActorRotation () ;
        FVector  GetActorScale () ;
        FQuat  GetActorRotationQuat () ;

        void SetActorLocation ( const FVector & InLocation );
        void SetActorLocation ( float inX, float inY, float inZ );
        void SetActorScale ( const FVector & InScale );
        void SetActorScale ( float inX, float inY, float inZ );
        void SetActorScale ( float InScale );
        void SetActorRotation ( const FVector & inRotation );
        void SetActorRotation ( const FQuat & inRotation );
        void SetActorRotation ( float inX, float inY, float inZ );


      
    protected:
        
        std::vector<CBaseComponent *> ActorComponents;
        CTransformComponent * RootComponent = nullptr;
        // State flags
        bool bIsCanTickAsAttached { false };
        bool bIsAttached { false };
        bool bIsPendingToDestroy { false };   
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