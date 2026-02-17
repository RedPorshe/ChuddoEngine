#pragma once

#include "Components/MeshComponent.h"

class CStaticMeshComponent : public CMeshComponent
{
    CHUDDO_DECLARE_CLASS(CStaticMeshComponent, CMeshComponent);
public:
    CStaticMeshComponent(CObject* inOwner = nullptr, const std::string& inDisplayName = "StaticMeshComponent");
    virtual ~CStaticMeshComponent() override;

    // Helper to create a cube mesh and assign to this component
    void CreateCubeMesh(float size = 1.0f);
};

REGISTER_CLASS_FACTORY(CStaticMeshComponent);
