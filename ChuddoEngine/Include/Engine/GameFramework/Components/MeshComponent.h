#pragma once

#include "Components/SceneComponent.h"
#include "Render/Mesh.h"
#include <memory>

class CMeshComponent : public CSceneComponent
{
    CHUDDO_DECLARE_CLASS(CMeshComponent, CSceneComponent);
public:
    CMeshComponent(CObject* inOwner = nullptr, const std::string& inDisplayName = "MeshComponent");
    virtual ~CMeshComponent();

    virtual void InitComponent() override;
    virtual void OnBeginPlay() override;

    // Set/Get mesh data (CPU-side)
    void SetMesh(std::unique_ptr<Mesh> mesh) { RuntimeMesh = std::move(mesh); }
    Mesh* GetMesh() const { return RuntimeMesh.get(); }

protected:
    std::unique_ptr<Mesh> RuntimeMesh;
};

REGISTER_CLASS_FACTORY(CMeshComponent);
