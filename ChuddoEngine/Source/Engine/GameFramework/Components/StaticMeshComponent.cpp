#include "Components/StaticMeshComponent.h"
#include "Render/Mesh.h"

CStaticMeshComponent::CStaticMeshComponent(CObject* inOwner, const std::string& inDisplayName)
    : Super(inOwner, inDisplayName)
{
}

CStaticMeshComponent::~CStaticMeshComponent()
{
}

void CStaticMeshComponent::CreateCubeMesh(float size)
{
    auto mesh = std::make_unique<CStaticMesh>();
    mesh->CreateCubeMesh(size);
    SetMesh(std::move(mesh));
}
