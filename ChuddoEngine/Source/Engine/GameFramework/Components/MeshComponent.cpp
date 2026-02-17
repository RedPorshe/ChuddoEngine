#include "Components/MeshComponent.h"
#include "Core/Engine.h"
#include "Render/Mesh.h"

CMeshComponent::CMeshComponent(CObject* inOwner, const std::string& inDisplayName)
    : Super(inOwner, inDisplayName)
{
}

CMeshComponent::~CMeshComponent()
{
}

void CMeshComponent::InitComponent()
{
    Super::InitComponent();
}

void CMeshComponent::OnBeginPlay()
{
    Super::OnBeginPlay();
}
