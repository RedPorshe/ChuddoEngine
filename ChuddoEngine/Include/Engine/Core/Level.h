#pragma once

#include "CoreMinimal.h"
#include "Core/Object.h"

class CActor;


class CLevel : public CObject
{
	public:
	CLevel(const CObject* Owner, const std::string& inName = "Level");
	
	virtual ~CLevel();

	// Level-specific methods can be added here
	virtual void  BeginPlay() ;
	virtual void  Tick(float DeltaTime);
	/*CActor* SpawnActor(const std::string& inName = "Actor" , const FTransform& SpawnTransworm = FTransform::Identity());
	void SpawnActor(CActor* NewActor, FTransform& SpawnTransform);*/
	
	// Template-based SpawnActor for spawning specific actor classes
	// Primary implementation with both transform and name
	template<typename ActorClass>
	ActorClass* SpawnActor(const std::string& inName, const FTransform& SpawnTransform = FTransform::Identity())
	{
		static_assert(std::is_base_of<CActor, ActorClass>::value, "ActorClass must be derived from CActor");
		
		auto NewActorUP = std::make_unique<ActorClass>(nullptr, inName);
		ActorClass* NewActor = NewActorUP.get();

		std::cout << "Spawning Actor '" << NewActor->GetName() << "' at location (" << SpawnTransform.Location << ") with Rotation ("
			<< SpawnTransform.Rotation << ") and Scale (" << SpawnTransform.Scale << ") in Level '"
			<< GetName() << "'." << std::endl;
		
		NewActor->SetActorTransform(SpawnTransform);
		AddOwnedObject(std::move(NewActorUP));
		RegisterActor(NewActor);

		return NewActor;
	}

	// Template-based SpawnActor with no arguments (default name and transform)
	template<typename ActorClass>
	ActorClass* SpawnActor()
	{
		return SpawnActor<ActorClass>(std::string("Actor"), FTransform::Identity());
	}

	// Template-based SpawnActor with only transform (default name)
	template<typename ActorClass>
	ActorClass* SpawnActorAt(const FTransform& SpawnTransform)
	{
		return SpawnActor<ActorClass>(std::string("Actor"), SpawnTransform);
	}
	
	void DestroyActor(CActor* ActorToDestroy);
	
	void DestroyActor(const std::string& ActorName);

	std::vector<CActor*> GetAllActors() const { return Actors; }

private:
	void RegisterActor(CActor* Actor);
	void ProcessPendingDestroyActors();
	std::vector<CActor*> Actors;
	std::unordered_map<std::string, CActor*> ActorNameMap;
	std::vector<CActor*> PendingDestroyActors;
	bool bIsTraveling = false;
};
