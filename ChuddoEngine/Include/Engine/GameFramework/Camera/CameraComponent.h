#pragma once

#include "Components/TransformComponent.h"
struct FCameraInfo;


class CCameraComponent : public CTransformComponent
    {
    CHUDDO_DECLARE_CLASS ( CCameraComponent, CTransformComponent );

    public:
        CCameraComponent ( CObject * inOwner = nullptr, const std::string & inDisplayName = "CameraComponent" );
        virtual ~CCameraComponent ();

        virtual void InitComponent () override;
        virtual void Tick ( float DeltaTime ) override;
        virtual void OnBeginPlay () override;
        FMat4 GetViewMatrix () const;
        FMat4 GetProjectionMatrix ( float AspectRatio ) const;
        FMat4 GetProjectionMatrix () const;

        // Для удобства - получить всю информацию о камере одним вызовом
        FCameraInfo GetCameraInfo ( float AspectRatio ) const;
        FCameraInfo GetCameraInfo () const;
        bool IsVisible () const { return bIsVisible; }
        void SetCameraVisible ( bool value ) { bIsVisible = value; }

        void SetFOV ( float value ) { FieldOfView = value; }
        void SetNearClipPlane ( float value ) { NearClipPlane = value; }
        void SetFarClipPlane ( float value ) { FarClipPlane = value; }
        float GetFOV () const { return FieldOfView; }
        float GetNearClipPlane () const { return NearClipPlane; }
        float GetFarClipPlane () const { return FarClipPlane; }
    protected:
        bool bIsVisible = true;
        // Camera-specific properties
        float FieldOfView = 90.0f;
        float NearClipPlane = 0.1f;
        float FarClipPlane = 1000.0f;

    };

REGISTER_CLASS_FACTORY ( CCameraComponent );