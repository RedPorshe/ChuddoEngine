#pragma once
#include "Components/BaseComponent.h"


class CSceneComponent : public CBaseComponent
	{
	CHUDDO_DECLARE_CLASS ( CSceneComponent, CBaseComponent );
	public:
		CSceneComponent ( CObject * inOwner = nullptr, const std::string & inDisplayName = "Object" );
		virtual ~CSceneComponent ();
		virtual void InitComponent () override;
		virtual void Tick ( float DeltaTime ) override;
		virtual void OnBeginPlay () override;
	};

REGISTER_CLASS_FACTORY ( CSceneComponent );
