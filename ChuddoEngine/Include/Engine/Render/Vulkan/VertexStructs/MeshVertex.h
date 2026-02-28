#pragma once
#include "CoreMinimal.h"
#include <vulkan/vulkan.h>


struct FMeshVertex
    {
    FVector Position;
    FVector Normal;
    FVector Color;
    FVector2D UV;

    static VkVertexInputBindingDescription GetBindingDescription ();
      

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions ();
        
    };