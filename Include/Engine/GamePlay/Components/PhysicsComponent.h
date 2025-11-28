#pragma once
#include "Engine/Utils/Math/AllMath.h"
#include "Engine/GamePlay/Components/Base/Component.h"

namespace CE
{
    class CPhysicsComponent : public CComponent
    {
    public:
        CPhysicsComponent(CObject* Owner = nullptr, FString Name = "PhysicsComponent");
        virtual ~CPhysicsComponent() = default;

        virtual void Update(float DeltaTime) override;

    void AddForce(const Math::Vector3f& Force);
    void AddImpulse(const Math::Vector3f& Impulse);
    void ApplyCollisionImpulse(const Math::Vector3f& Impulse);
    void ClearForces();

    void SetUseGravity(bool bUse) { bUseGravity = bUse; }
    void SetMass(float NewMass) { Mass = NewMass; }
    void SetFriction(float NewFriction) { Friction = NewFriction; }

    Math::Vector3f GetVelocity() const { return Velocity; }
    void SetVelocity(const Math::Vector3f& NewVelocity) { Velocity = NewVelocity; }
    float GetMass() const { return Mass; }

    protected:
        void SimulatePhysics(float DeltaTime);

        bool bIsKinematic = false;
        bool bUseGravity = true;

        float Mass = 1.0f;
        float Friction = 0.1f;

        Math::Vector3f Velocity = Math::Vector3f(0.0f);
        Math::Vector3f Acceleration = Math::Vector3f(0.0f);
        Math::Vector3f AccumulatedForce = Math::Vector3f(0.0f);
        Math::Vector3f AccumulatedTorque = Math::Vector3f(0.0f);
        Math::Vector3f AngularVelocity = Math::Vector3f(0.0f);
    };
}
