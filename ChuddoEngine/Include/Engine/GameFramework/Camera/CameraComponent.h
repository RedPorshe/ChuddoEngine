#pragma once

#include "Components/TransformComponent.h"

class CCameraComponent : public CTransformComponent
    {
    CHUDDO_DECLARE_CLASS ( CCameraComponent, CTransformComponent );

    public:
        CCameraComponent ( CObject * inOwner = nullptr, const std::string & inDisplayName = "CameraComponent" );
        virtual ~CCameraComponent ();

        virtual void InitComponent () override;
        virtual void Tick ( float DeltaTime ) override;
        virtual void OnBeginPlay () override;

        void SetFOV ( float value ) { FieldOfView = value; }
        void SetNearClipPlane ( float value ) { NearClipPlane = value; }
        void SetFarClipPlane ( float value ) { FarClipPlane = value; }
        float GetFOV () const { return FieldOfView; }
        float GetNearClipPlane () const { return NearClipPlane; }
        float GetFarClipPlane () const { return FarClipPlane; }
    protected:

        // Camera-specific properties
        float FieldOfView = 90.0f;
        float NearClipPlane = 0.1f;
        float FarClipPlane = 1000.0f;

    };

REGISTER_CLASS_FACTORY ( CCameraComponent );