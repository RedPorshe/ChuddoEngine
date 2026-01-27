#include "Components/BaseComponent.h"
CBaseComponent::CBaseComponent(const CObject* Owner, const std::string& inName)
	: CObject(Owner, inName)
{
	std::cout << "BaseComponent '" << GetName() << "' created." << std::endl;
	SetCanTick(true);
	
	SetActive(true);
}
CBaseComponent::~CBaseComponent()
{
	std::cout << "BaseComponent '" << GetName() << "' destroyed." << std::endl;
}
void CBaseComponent::BeginPlay()
{
	InitializeComponent();
	if (bIsActive && bIsInitialized)
	{		
		if (HasOwnedObjects())
		{
			for (size_t i = 0; i < GetOwnedObjectsCount(); ++i)
			{
				CObject* Obj = OwnedObjects[i].get();
				if (Obj)
				{
					CBaseComponent* ComponentObj = dynamic_cast<CBaseComponent*>(Obj);
					if (ComponentObj)
					{
						std::cout << "Starting BeginPlay on owned Component: " << ComponentObj->GetName() << " Owner is : " << GetName() << std::endl;
						ComponentObj->BeginPlay();
					}
				}
			}
		}
	}
}
void CBaseComponent::Tick(float DeltaTime)
{
	if(bIsCanTick && bIsInitialized)
	{		
		if (HasOwnedObjects())
		{
			for (size_t i = 0; i < GetOwnedObjectsCount(); ++i)
			{
				CObject* Obj = OwnedObjects[i].get();
				if (Obj)
				{
					CBaseComponent* ComponentObj = dynamic_cast<CBaseComponent*>(Obj);
					if (ComponentObj)
					{
						std::cout << "Ticking owned Component: " << ComponentObj->GetName() << " Owner is : " << GetName() << std::endl;
						ComponentObj->Tick(DeltaTime);
					}
				}
			}
		}
	}	
}

void CBaseComponent::InitializeComponent()
{	
	// Initialization logic here
	bIsInitialized = true;
}
