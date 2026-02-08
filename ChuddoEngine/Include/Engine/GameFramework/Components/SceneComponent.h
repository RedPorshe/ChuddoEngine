#pragma once
#include "Components/TransformComponent.h"


class CSceneComponent : public CTransformComponent
	{
	CHUDDO_DECLARE_CLASS ( CSceneComponent, CTransformComponent );
	public:
		CSceneComponent ( CObject * inOwner = nullptr, const std::string & inDisplayName = "Object" );
		virtual ~CSceneComponent ();
		virtual void InitComponent () override;
		virtual void Tick ( float DeltaTime ) override;
		virtual void OnBeginPlay () override;	
	};

REGISTER_CLASS_FACTORY ( CSceneComponent );
