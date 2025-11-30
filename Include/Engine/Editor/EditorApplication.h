#pragma once

#include "Engine/Editor/UI/PropertyEditor.h"
#include "Engine/Application/Application.h"


  enum class EditorMode
  {
    Play,
    Edit
  };

  class EditorApplication : public Application
  {
   public:
    EditorApplication(AppInfo* info);
    virtual ~EditorApplication() override;

    void Initialize() override;
    void Run() override;
    void Shutdown() override;

    // Editor-specific methods
    void SetEditorMode(EditorMode mode) { m_EditorMode = mode; }
    EditorMode GetEditorMode() const { return m_EditorMode; }
    void ToggleEditorMode();

    // UI methods
    void InitializeImGui();
    void ShutdownImGui();
    void BeginImGuiFrame();
    void EndImGuiFrame();
    void RenderEditorUI();

   private:
    void ProcessEditorInput();
    void RenderPlayMode();
    void RenderEditMode();

    // UI components
    void RenderMainMenuBar();
    void RenderViewport();
    void RenderOutliner();
    void RenderPropertyPanel();

    EditorMode m_EditorMode = EditorMode::Edit;
    bool m_ShowOutliner = true;
    bool m_ShowPropertyPanel = true;
    bool m_ShowViewport = true;

    CObject* m_SelectedObject = nullptr;

    PropertyEditor m_PropertyEditor;
  };
