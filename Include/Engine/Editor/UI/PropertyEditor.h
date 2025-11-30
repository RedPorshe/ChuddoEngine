#pragma once

#include <unordered_map>
#include <vector>

#include "Engine/Core/Object.h"
#include "Engine/Core/Reflection.h"

namespace CE
{
  class PropertyEditor
  {
   public:
    PropertyEditor();
    ~PropertyEditor();

    void Initialize();
    void Shutdown();
    void Render(CObject* selectedObject);
    void SetSelectedObject(CObject* object);

   private:
    void RenderProperty(const Property& property, void* object);
    void RenderBoolProperty(const Property& property, void* object);
    void RenderIntProperty(const Property& property, void* object);
    void RenderFloatProperty(const Property& property, void* object);
    void RenderDoubleProperty(const Property& property, void* object);
    void RenderStringProperty(const Property& property, void* object);
    void RenderVector2Property(const Property& property, void* object);
    void RenderVector3Property(const Property& property, void* object);
    void RenderVector4Property(const Property& property, void* object);
    void RenderQuaternionProperty(const Property& property, void* object);
    void RenderColorProperty(const Property& property, void* object);
    void RenderCustomProperty(const Property& property, void* object);

    CObject* m_SelectedObject = nullptr;
    bool m_IsInitialized = false;
  };
}  // namespace CE
