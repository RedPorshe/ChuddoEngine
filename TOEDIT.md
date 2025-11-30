# TOEDIT: Transform Engine into Unreal Engine-like System with World Editor

## Information Gathered

* \[x] **Current Engine Structure**: C++ game engine using Vulkan for rendering, SDL2 for windowing, custom math from Engine\Utils\Math for math. Architecture includes Application, GameInstance, World, Level, Actor, Component classes, similar to Unreal Engine.
* \[x] **Rendering**: Vulkan-based with managers for buffers, pipelines, etc. Uses SDL2 for window creation and input.
* \[x] **Gameplay**: Hierarchical structure with Worlds containing Levels, Levels containing Actors, Actors containing Components (e.g., Camera, Mesh, Input).
* \[x] **Input**: SDL2-based input system.
* \[x] **Build System**: CMake with support for MinGW/Windows, shader compilation, asset copying, SDL2 and custom Slate-like UI integration.
* \[x] **Key Files Analyzed**: main.cpp, Application.cpp, GameInstance.cpp, RenderSystem.h, VulkanContext.h, Level.h, Component.h, World.h, EditorApplication.h/.cpp.
* \[x] Create a Slate-like tool from the Unreal Editor for the world editor

## Plan

Transform the engine into an Unreal Engine-like system by adding a world editor with UI for editing actors, components, and world properties. This involves integrating a GUI library, adding editor modes, and modifying the application loop to support editing.

### Phase 0: Create Slate-like UI Framework

* \[ ] Create custom Slate-like UI framework for editor UI (docking, windows, property editors).
* \[ ] Integrate custom Slate-like UI immediate-mode GUI rendering.
* \[ ] Implement basic UI components: windows, panels, buttons, text inputs, etc.
* \[ ] Ensure UI rendering integrates with Vulkan pipeline.

### Phase 1: Implement Reflection System (like Unreal Engine UPROPERTY, UCLASS)

* \[x] Create reflection macros (UCLASS, UPROPERTY, UFUNCTION) for runtime type information.
* \[x] Implement property system for serialization and editor inspection.
* \[x] Add metadata system for editor hints (display names, categories, etc.).
* \[x] Support for basic types (int, float, Vector3, FString) and custom types.
* \[ ] Integrate reflection with serialization for level/world saving/loading.

### Phase 2: Integrate GUI Library for Editor UI

* \[ ] Replace GLFW with SDL2 for better cross-platform support and potential for advanced UI (SDL supports more features than GLFW for editors).
* \[ ] Create custom Slate-like UI framework for editor UI (docking, windows, property editors).
* \[ ] Modify CMakeLists.txt to include SDL2 
### Phase 3: Add Editor Mode and UI Framework

* \[ ] Create EditorApplication class inheriting from Application, with editor-specific logic.
* \[ ] Add EditorMode enum (Play, Edit) to toggle between gameplay and editing.
* \[ ] Implement basic editor UI: Main menu, viewport, outliner (actor hierarchy), property panel.
* \[ ] Modify Application::Run() to handle editor input and rendering.

### Phase 4: Implement Actor Selection and Manipulation

* \[ ] Add selection system: Raycasting from mouse to select actors in viewport.
* \[ ] Implement gizmos for translate, rotate, scale operations on selected actors.
* \[ ] Add keyboard shortcuts (e.g., W/E/R for gizmo modes, Delete to remove actors).

### Phase 5: Property Editor and Component Editing

* \[ ] Create PropertyEditor class to display and edit actor/component properties.
* \[ ] Support for basic types (int, float, Vector3, etc.) and custom editors.
* \[ ] Use custom Slate-like UI widgets for property editing.

### Phase 6: World and Level Editing

* \[ ] Add level loading/saving (serialize actors to JSON or custom format).
* \[ ] Implement drag-and-drop for placing new actors in the world.
* \[ ] Add world settings editor (lighting, physics, etc.).

### Phase 7: Asset Management and Integration

* \[ ] Basic asset browser for meshes, textures.
* \[ ] Integrate with existing ObjLoader for mesh assets.
* \[ ] Add hot-reloading for assets during editing.

### Phase 8: Advanced Editor Features

* \[ ] Undo/Redo system.
* \[ ] Multi-selection and group operations.
* \[ ] Blueprint-like visual scripting (future extension).
* \[ ] Play-in-editor mode.

## Dependent Files to be Edited/Created

* \[ ] **CMakeLists.txt**: Update to use SDL2 instead of GLFW, add custom Slate-like UI.
* \[ ] **Include/Engine/Application/Application.h/.cpp**: Add editor mode support, modify Run() loop.
* \[ ] **Include/Engine/Core/Rendering/Vulkan/Integration/RenderSystem.h/.cpp**: Update to use SDL2 window.
* \[ ] **Include/Engine/Core/Rendering/Vulkan/Core/VulkanContext.h/.cpp**: Change window creation to SDL2.
* \[ ] **New: Include/Engine/Editor/EditorApplication.h/.cpp**: Editor-specific application class.
* \[ ] **New: Include/Engine/Editor/UI/\*.h/.cpp**: ImGui integration, property editor, outliner, etc.
* \[ ] **New: Include/Engine/Editor/Selection/\*.h/.cpp**: Actor selection and gizmo systems.
* \[ ] **Source/Engine/Input/InputSystem.cpp**: Update for SDL2 input.
* \[ ] **New: Source/Engine/Editor/**/\*.cpp\*\*: Implementations for editor features.

## Followup Steps

* \[ ] Test editor integration with existing gameplay.
* \[ ] Profile performance impact of editor UI on rendering.
* \[ ] Add unit tests for new editor classes.
* \[ ] Document editor usage for developers.
* \[ ] Consider packaging as separate executable or mode.

## Risks and Considerations

* \[ ] SDL2 integration may require significant changes to Vulkan surface creation.
* \[ ] Custom Slate-like UI rendering needs to be integrated with Vulkan pipeline.
* \[ ] Ensure editor doesn't break existing gameplay functionality.
* \[ ] Performance: Editor UI should not significantly impact frame rates.
