#pragma once
#include "Components/BaseComponent.h"


//Forward declarations
class CActor;

class CSceneComponent :
    public CBaseComponent
    {
    public:
        CSceneComponent ( const CObject * Owner, const std::string & inName = "SceneComponent" );
        virtual ~CSceneComponent ();

        virtual void BeginPlay () override;
        virtual void Tick ( float DeltaTime ) override;
    protected:
        CActor * m_OwnerActor = nullptr;
        CSceneComponent * m_OwnerComponent = nullptr;
    };

