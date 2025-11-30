#include "Engine/Editor/UI/PropertyEditor.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

#include "Engine/Core/Reflection.h"
#include "Engine/Core/Object.h"
#include "Engine/Utils/Math/AllMath.h"

namespace CE
{
  PropertyEditor::PropertyEditor()
  {
  }

  PropertyEditor::~PropertyEditor()
  {
    if (m_IsInitialized)
    {
      Shutdown();
    }
  }

  void PropertyEditor::Initialize()
  {
    if (m_IsInitialized) return;

    // ImGui is already initialized in EditorApplication
    m_IsInitialized = true;
    CE_CORE_DEBUG("PropertyEditor initialized");
  }

  void PropertyEditor::Shutdown()
  {
    if (!m_IsInitialized) return;

    m_IsInitialized = false;
    CE_CORE_DEBUG("PropertyEditor shutdown");
  }

  void PropertyEditor::Render(CObject* selectedObject)
  {
    if (!m_IsInitialized) return;

    SetSelectedObject(selectedObject);

    ImGui::Begin("Property Editor");

    if (!m_SelectedObject)
    {
      ImGui::Text("No object selected");
      ImGui::End();
      return;
    }

    const ClassInfo* classInfo = m_SelectedObject->GetClassInfo();
    if (!classInfo)
    {
      ImGui::Text("No reflection data available");
      ImGui::End();
      return;
    }

    ImGui::Text("Object: %s", m_SelectedObject->GetName().c_str());
    ImGui::Separator();

    // Group properties by category
    std::unordered_map<std::string, std::vector<const Property*>> categorizedProperties;

    for (const auto& prop : classInfo->GetProperties())
    {
      categorizedProperties[prop->GetMeta().Category].push_back(prop.get());
    }

    for (const auto& [category, properties] : categorizedProperties)
    {
      if (!category.empty() && category != "Default")
      {
        ImGui::Text("%s", category.c_str());
        ImGui::Separator();
      }

      for (const Property* prop : properties)
      {
        RenderProperty(*prop, m_SelectedObject);
      }

      if (!category.empty())
      {
        ImGui::Separator();
      }
    }

    ImGui::End();
  }

  void PropertyEditor::SetSelectedObject(CObject* object)
  {
    m_SelectedObject = object;
  }

  void PropertyEditor::RenderProperty(const Property& property, void* object)
  {
    switch (property.GetType())
    {
      case EPropertyType::Bool:
        RenderBoolProperty(property, object);
        break;
      case EPropertyType::Int:
        RenderIntProperty(property, object);
        break;
      case EPropertyType::Float:
        RenderFloatProperty(property, object);
        break;
      case EPropertyType::Double:
        RenderDoubleProperty(property, object);
        break;
      case EPropertyType::String:
        RenderStringProperty(property, object);
        break;
      case EPropertyType::Vector2:
        RenderVector2Property(property, object);
        break;
      case EPropertyType::Vector3:
        RenderVector3Property(property, object);
        break;
      case EPropertyType::Vector4:
        RenderVector4Property(property, object);
        break;
      case EPropertyType::Quaternion:
        RenderQuaternionProperty(property, object);
        break;
      case EPropertyType::Color:
        RenderColorProperty(property, object);
        break;
      case EPropertyType::Custom:
        RenderCustomProperty(property, object);
        break;
      default:
        ImGui::Text("Unsupported property type: %s", property.GetName().c_str());
        break;
    }
  }

  void PropertyEditor::RenderBoolProperty(const Property& property, void* object)
  {
    bool* value = static_cast<bool*>(property.GetValuePtr(object));
    ImGui::Checkbox(property.GetMeta().DisplayName.c_str(), value);
  }

  void PropertyEditor::RenderIntProperty(const Property& property, void* object)
  {
    int* value = static_cast<int*>(property.GetValuePtr(object));
    const PropertyMeta& meta = property.GetMeta();

    if (meta.MinValue != meta.MaxValue)
    {
      ImGui::SliderInt(property.GetMeta().DisplayName.c_str(), value,
                       static_cast<int>(meta.MinValue), static_cast<int>(meta.MaxValue));
    }
    else
    {
      ImGui::InputInt(property.GetMeta().DisplayName.c_str(), value);
    }
  }

  void PropertyEditor::RenderFloatProperty(const Property& property, void* object)
  {
    float* value = static_cast<float*>(property.GetValuePtr(object));
    const PropertyMeta& meta = property.GetMeta();

    if (meta.MinValue != meta.MaxValue)
    {
      ImGui::SliderFloat(property.GetMeta().DisplayName.c_str(), value, meta.MinValue, meta.MaxValue);
    }
    else
    {
      ImGui::InputFloat(property.GetMeta().DisplayName.c_str(), value);
    }
  }

  void PropertyEditor::RenderDoubleProperty(const Property& property, void* object)
  {
    double* value = static_cast<double*>(property.GetValuePtr(object));
    ImGui::InputDouble(property.GetMeta().DisplayName.c_str(), value);
  }

  void PropertyEditor::RenderStringProperty(const Property& property, void* object)
  {
    FString* value = static_cast<FString*>(property.GetValuePtr(object));
    static char buffer[256];
    strcpy_s(buffer, value->c_str());
    if (ImGui::InputText(property.GetMeta().DisplayName.c_str(), buffer, sizeof(buffer)))
    {
      *value = buffer;
    }
  }

  void PropertyEditor::RenderVector2Property(const Property& property, void* object)
  {
    CE::Math::Vector2f* value = static_cast<CE::Math::Vector2f*>(property.GetValuePtr(object));
    ImGui::InputFloat2(property.GetMeta().DisplayName.c_str(), &value->x);
  }

  void PropertyEditor::RenderVector3Property(const Property& property, void* object)
  {
    CE::Math::Vector3f* value = static_cast<CE::Math::Vector3f*>(property.GetValuePtr(object));
    ImGui::InputFloat3(property.GetMeta().DisplayName.c_str(), &value->x);
  }

  void PropertyEditor::RenderVector4Property(const Property& property, void* object)
  {
    CE::Math::Vector4f* value = static_cast<CE::Math::Vector4f*>(property.GetValuePtr(object));
    ImGui::InputFloat4(property.GetMeta().DisplayName.c_str(), &value->x);
  }

  void PropertyEditor::RenderQuaternionProperty(const Property& property, void* object)
  {
    CE::Math::Quaternionf* value = static_cast<CE::Math::Quaternionf*>(property.GetValuePtr(object));
    ImGui::InputFloat4(property.GetMeta().DisplayName.c_str(), &value->x);
  }

  void PropertyEditor::RenderColorProperty(const Property& property, void* object)
  {
    CE::Math::Color* value = static_cast<CE::Math::Color*>(property.GetValuePtr(object));
    ImGui::ColorEdit4(property.GetMeta().DisplayName.c_str(), &value->r);
  }

  void PropertyEditor::RenderCustomProperty(const Property& property, void* object)
  {
    ImGui::Text("%s: %s", property.GetMeta().DisplayName.c_str(),
                property.GetValueAsString(object).c_str());
  }
}  // namespace CE
