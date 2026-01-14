#include "Core/Level.h"
#include "Actors/Actor.h"

CLevel::CLevel(const CObject* Owner, const std::string& inName) : CObject(Owner, inName)
{

	
}



CLevel::~CLevel()
{
	// Destroy all actors in this level before destroying sublevels
	for (auto act : Actors)
	{
		DestroyActor(act);
	}
	ProcessPendingDestroyActors();

	// Now destroy sublevels - they will be destroyed by ClearOwnedObjects when this destructor finishes
	
	std::cout << "Level '" << GetName() << "' destroyed." << std::endl;

}

void CLevel::BeginPlay()
{
	std::cout << "Begin Play on Level: " << GetName() << std::endl;

	if (HasOwnedObjects())
	{
		for (size_t i = 0; i < GetOwnedObjectsCount(); ++i)
		{
			CObject* Obj = OwnedObjects[i].get();
			if (Obj)
			{
				CLevel* LevelObj = dynamic_cast<CLevel*>(Obj);
				if (LevelObj)
				{
					std::cout << "Starting BeginPlay on owned Level as sublevel: " << LevelObj->GetName() << " Owner is : " << GetName() << std::endl;
					LevelObj->BeginPlay();
				}				
			}
		}
	}
	if (!Actors.empty())
	{
		for (CActor* Actor : Actors)
		{
			if (Actor && !Actor->IsPendingDestroy())
			{
				Actor->BeginPlay();
			}			
		}
	}
}

void CLevel::Tick(float DeltaTime)
{
	std::cout << "Tick on Level: " << GetName() << " with DeltaTime: " << DeltaTime << std::endl;
	if (HasOwnedObjects())
	{
		for (size_t i = 0; i < GetOwnedObjectsCount(); ++i)
		{
			CObject* Obj = OwnedObjects[i].get();
			if (Obj)
			{
				CLevel* LevelObj = dynamic_cast<CLevel*>(Obj);
				if (LevelObj)
				{
					std::cout << "Ticking owned Level as sublevel : " << LevelObj->GetName() << " Owner is : " << GetName() << std::endl;
					LevelObj->Tick(DeltaTime);
				}				
			}
		}
	}
	if (!Actors.empty())
	{
		std::cout << "Ticking Actors in Level: " << GetName() << std::endl;
		for (CActor* Actor : Actors)
		{
			if (Actor)
			{
				Actor->Tick(DeltaTime);
			}
			
		}
	}
	ProcessPendingDestroyActors();
}

//void CLevel::SpawnActor(CActor* NewActor, FTransform& SpawnTransform)
//{
//	auto NewActorUP = std::make_unique<CActor>(this,NewActor->GetName());
//	auto NewActors = NewActorUP.get();
//	
//	NewActors->SetActorTransform(SpawnTransform);
//	AddOwnedObject(std::move(NewActorUP));
//	RegisterActor(NewActors);
//}
//
//CActor* CLevel::SpawnActor(const std::string& inName, const FTransform& SpawnTransform)
//{
//	// create actor with no owner (nullptr) - ownership will be transferred to this level via AddOwnedObject
//	auto NewActorUP = std::make_unique<CActor>(nullptr, inName);
//	CActor* NewActor = NewActorUP.get();
//
//	std::cout << "Spawning Actor '" << NewActor->GetName() << "' at location (" << SpawnTransform.Location << ") whith Rotation ("<< SpawnTransform.Rotation <<") and Scale ("<<SpawnTransform.Scale <<") in Level '"
//		<< GetName() << "'." << std::endl;
//	NewActor->SetActorTransform(SpawnTransform);
//	// Additional initialization for the actor can be added here
//
//	// register and take ownership - this will set the correct OwnerObject
//	AddOwnedObject(std::move(NewActorUP));
//	RegisterActor(NewActor);
//
//	return NewActor;
//}

void CLevel::DestroyActor(CActor* ActorToDestroy)
{
	if (ActorToDestroy)
	{
		ActorToDestroy->SetPendingDestroy(true);
		PendingDestroyActors.push_back(ActorToDestroy);
		std::cout << "Marked Actor '" << ActorToDestroy->GetName() << "' for destruction in Level '"
			<< GetName() << "'." << std::endl;		
	}
}

void CLevel::DestroyActor(const std::string& ActorName)
{
	CActor* ActorToDestroy = nullptr;
	auto it = ActorNameMap.find(ActorName);
	if (it != ActorNameMap.end())
	{
		ActorToDestroy = it->second;
		if (ActorToDestroy)
		{
			DestroyActor(ActorToDestroy);
		}
	}
	
}

void CLevel::RegisterActor(CActor* Actor)
{
	Actors.push_back(Actor);
	ActorNameMap[Actor->GetName()] = Actor;
}

void CLevel::ProcessPendingDestroyActors()
{
	for (CActor* Actor : PendingDestroyActors)
	{
		auto it = std::find(Actors.begin(), Actors.end(), Actor);
		if (it != Actors.end())
		{		
			Actors.erase(it);
			ActorNameMap.erase(Actor->GetName());

			// Prefer owner-managed deletion to avoid double-delete: if owner holds unique_ptr, ask it to delete
			const CObject* Owner = Actor->GetOwner();
			if (Owner && CObject::s_AliveObjects.find(Owner) != CObject::s_AliveObjects.end())
			{
				const_cast<CObject*>(Owner)->RemoveOwnedObject(Actor, true);
			}
			else
			{
				// fallback: no owner or owner already dead
				delete Actor;
			}
		}
	}
	PendingDestroyActors.clear();
}
