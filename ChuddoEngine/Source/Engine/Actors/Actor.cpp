#include "Actors/Actor.h"
#include "Components/BaseComponent.h"

CActor::CActor(const CObject* Owner, const std::string& inName) : CObject(Owner, inName)
{
	std::cout << "Actor '" << GetName() << "' created." << std::endl;
	SetCanTick(true);
	
}
CActor::CActor(const std::string& inName) : CObject(inName)
{
}
CActor::~CActor()
{
	std::cout << "Actor '" << GetName() << "' destroyed." << std::endl;
}
void CActor::BeginPlay()
{
	
		std::cout << "Begin Play on Actor: " << GetName() << std::endl;
		if (HasOwnedObjects())
		{
			for (size_t i = 0; i < GetOwnedObjectsCount(); ++i)
			{
				CObject* Obj = OwnedObjects[i].get();
				if (Obj)
				{
					CBaseComponent* ActorObj = dynamic_cast<CBaseComponent*>(Obj);
					if (ActorObj)
					{
						std::cout << "Starting BeginPlay on owned Component: " << ActorObj->GetName() << " Owner is : " << GetName() << std::endl;
						ActorObj->BeginPlay();
					}
				}
			}
		}
		bIsInitialized = true;
}
void CActor::Tick(float DeltaTime)
{
	if (bIsCanTick && bIsInitialized)
	{
		std::cout << "Tick on Actor: " << GetName() << " with DeltaTime: " << DeltaTime << std::endl;
		if (HasOwnedObjects())
		{
			for (size_t i = 0; i < GetOwnedObjectsCount(); ++i)
			{
				CObject* Obj = OwnedObjects[i].get();
				if (Obj)
				{
					CBaseComponent* ActorObj = dynamic_cast<CBaseComponent*>(Obj);
					if (ActorObj)
					{
						std::cout << "Ticking owned Component: " << ActorObj->GetName() << " Owner is : " << GetName() << std::endl;
						ActorObj->Tick(DeltaTime);
					}
				}
			}
		}
	}
}

void CActor::SetActorRotation(float Pitch, float Yaw, float Roll)
{
	// Convert degrees to radians
	float RadPitch = Pitch * (3.14159265f / 180.0f);
	float RadYaw = Yaw * (3.14159265f / 180.0f);
	float RadRoll = Roll * (3.14159265f / 180.0f);
	// Calculate quaternion components
	float cy = cos(RadYaw * 0.5f);
	float sy = sin(RadYaw * 0.5f);
	float cp = cos(RadPitch * 0.5f);
	float sp = sin(RadPitch * 0.5f);
	float cr = cos(RadRoll * 0.5f);
	float sr = sin(RadRoll * 0.5f);
	FQuat NewRotation;
	NewRotation.w = cr * cp * cy + sr * sp * sy;
	NewRotation.x = sr * cp * cy - cr * sp * sy;
	NewRotation.y = cr * sp * cy + sr * cp * sy;
	NewRotation.z = cr * cp * sy - sr * sp * cy;
	SetActorRotation(NewRotation);
}

