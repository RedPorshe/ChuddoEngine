#include "Engine/Editor/UI/PropertyEditor.h"

#include "Engine/Core/Reflection.h"
#include "Engine/Core/Object.h"
#include "Engine/Utils/Math/AllMath.h"


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
    CORE_DEBUG("PropertyEditor initialized");
  }

  void PropertyEditor::Shutdown()
  {
    if (!m_IsInitialized) return;

    m_IsInitialized = false;
    CORE_DEBUG("PropertyEditor shutdown");
  }

  void PropertyEditor::Render(CObject* selectedObject)
  {
    if (!m_IsInitialized) return;

    SetSelectedObject(selectedObject);

    // TODO: Implement Slate-based rendering
    // For now, this is a placeholder
    if (!m_SelectedObject)
    {
      // Render "No object selected" using Slate widgets
      return;
    }

    const ClassInfo* classInfo = m_SelectedObject->GetClassInfo();
    if (!classInfo)
    {
      // Render "No reflection data available" using Slate widgets
      return;
    }

    // TODO: Render object name and properties using Slate widgets
  }

  void PropertyEditor::SetSelectedObject(CObject* object)
  {
    m_SelectedObject = object;
  }


