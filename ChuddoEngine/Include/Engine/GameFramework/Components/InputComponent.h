#pragma once

#include "Components/BaseComponent.h"

class CPawn;

class CInputComponent : public CBaseComponent
	{
	CHUDDO_DECLARE_CLASS ( CInputComponent, CBaseComponent );
	public:
		CInputComponent (CObject* inOwner = nullptr, const std::string& inDysplayName = "InputComponent");
		virtual ~CInputComponent ();
		virtual void InitComponent () override;
		virtual void Tick ( float DeltaTime ) override;
		virtual void OnBeginPlay () override;
	};

REGISTER_CLASS_FACTORY ( CInputComponent );