#include "Engine/GamePlay/Components/PhysicsComponent.h"
#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/World/Levels/Level.h"

namespace CE
{
    CPhysicsComponent::CPhysicsComponent(CObject* Owner, FString Name)
        : CComponent(Owner, Name)
    {
    }

    void CPhysicsComponent::Update(float DeltaTime)
    {
        if (!bIsKinematic)
        {
            SimulatePhysics(DeltaTime);
        }
    }

    void CPhysicsComponent::AddForce(const Math::Vector3f& Force)
    {
        AccumulatedForce += Force;
    }

    void CPhysicsComponent::AddImpulse(const Math::Vector3f& Impulse)
    {
        Velocity += Impulse / Mass;
    }

    void CPhysicsComponent::ApplyCollisionImpulse(const Math::Vector3f& Impulse)
    {
        Velocity += Impulse / Mass;
    }

    void CPhysicsComponent::ClearForces()
    {
        AccumulatedForce = Math::Vector3f(0.0f);
        AccumulatedTorque = Math::Vector3f(0.0f);
    }

    void CPhysicsComponent::SimulatePhysics(float DeltaTime)
    {
        // Get gravity from level
        Math::Vector3f gravity = Math::Vector3f(0.0f, -9.81f, 0.0f); // Default gravity
        if (auto* actor = dynamic_cast<CActor*>(GetOwner()))
        {
            if (auto* level = actor->GetLevel())
            {
                gravity = level->GetGravity();
            }
        }

        // Apply gravity if enabled
        if (bUseGravity)
        {
            Acceleration = gravity;
        }
        else
        {
            Acceleration = Math::Vector3f(0.0f);
        }

        // Apply accumulated forces
        Acceleration += AccumulatedForce / Mass;

        // Integrate velocity
        Velocity += Acceleration * DeltaTime;

        // Apply friction
        Velocity *= (1.0f - Friction * DeltaTime);

        // Integrate position
        if (auto* actor = dynamic_cast<CActor*>(GetOwner()))
        {
            Math::Vector3f currentPos = actor->GetActorLocation();
            actor->SetActorLocation(currentPos + Velocity * DeltaTime);

            // Integrate rotation
            Math::Vector3f currentRot = actor->GetActorRotation();
            actor->SetActorRotation(currentRot + AngularVelocity * DeltaTime);

            // Apply angular damping
            AngularVelocity *= (1.0f - Friction * DeltaTime);
        }

        // Clear accumulated forces
        ClearForces();
    }
} // namespace CE
