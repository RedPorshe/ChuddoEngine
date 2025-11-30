# TODO: Transform Engine into Unreal Engine-like System with World Editor

## Information Gathered
- **Current Engine Structure**: C++ game engine using Vulkan for rendering, GLFW for windowing, GLM for math. Architecture includes Application, GameInstance, World, Level, Actor, Component classes, similar to Unreal Engine.
- **Rendering**: Vulkan-based with managers for buffers, pipelines, etc. Uses GLFW for window creation and input.
- **Gameplay**: Hierarchical structure with Worlds containing Levels, Levels containing Actors, Actors containing Components (e.g., Camera, Mesh, Input).
- **Input**: GLFW-based input system.
- **Build System**: CMake with support for MinGW/Windows, shader compilation, asset copying.
- **Key Files Analyzed**: main.cpp, Application.cpp, GameInstance.cpp, RenderSystem.h, VulkanContext.h, Level.h, Component.h, World.h.

## Plan
Transform the engine into an Unreal Engine-like system by adding a world editor with UI for editing actors, components, and world properties. This involves integrating a GUI library, adding editor modes, and modifying the application loop to support editing.

### Phase 1: Integrate GUI Library for Editor UI
- [x] Replace GLFW with SDL2 for better cross-platform support and potential for advanced UI (SDL supports more features than GLFW for editors).
- Create custom Slate-like UI framework for editor UI (docking, windows, property editors).
- [x] Modify CMakeLists.txt to include SDL2 and custom Slate-like UI dependencies.

### Phase 2: Add Editor Mode and UI Framework
- [x] Create EditorApplication class inheriting from Application, with editor-specific logic.
- [x] Add EditorMode enum (Play, Edit) to toggle between gameplay and editing.
- [x] Implement basic editor UI: Main menu, viewport, outliner (actor hierarchy), property panel.
- [x] Modify Application::Run() to handle editor input and rendering.

### Phase 3: Implement Actor Selection and Manipulation
- Add selection system: Raycasting from mouse to select actors in viewport.
- Implement gizmos for translate, rotate, scale operations on selected actors.
- Add keyboard shortcuts (e.g., W/E/R for gizmo modes, Delete to remove actors).

### Phase 4: Property Editor and Component Editing
- Create PropertyEditor class to display and edit actor/component properties.
- Support for basic types (int, float, Vector3, etc.) and custom editors.
- Use custom Slate-like UI widgets for property editing.

### Phase 5: World and Level Editing
- Add level loading/saving (serialize actors to JSON or custom format).
- Implement drag-and-drop for placing new actors in the world.
- Add world settings editor (lighting, physics, etc.).

### Phase 6: Asset Management and Integration
- Basic asset browser for meshes, textures.
- Integrate with existing ObjLoader for mesh assets.
- Add hot-reloading for assets during editing.

### Phase 7: Advanced Editor Features
- Undo/Redo system.
- Multi-selection and group operations.
- Blueprint-like visual scripting (future extension).
- Play-in-editor mode.

## Dependent Files to be Edited/Created
- **CMakeLists.txt**: Update to use SDL2 instead of GLFW, add custom Slate-like UI.
- **Include/Engine/Application/Application.h/.cpp**: Add editor mode support, modify Run() loop.
- **Include/Engine/Core/Rendering/Vulkan/Integration/RenderSystem.h/.cpp**: Update to use SDL2 window.
- **Include/Engine/Core/Rendering/Vulkan/Core/VulkanContext.h/.cpp**: Change window creation to SDL2.
- **New: Include/Engine/Editor/EditorApplication.h/.cpp**: Editor-specific application class.
- **New: Include/Engine/Editor/UI/*.h/.cpp**: Slate-like UI integration, property editor, outliner, etc.
- **New: Include/Engine/Editor/Selection/*.h/.cpp**: Actor selection and gizmo systems.
- **Source/Engine/Input/InputSystem.cpp**: Update for SDL2 input.
- **New: Source/Engine/Editor/**/*.cpp**: Implementations for editor features.

## Followup Steps
- Test editor integration with existing gameplay.
- Profile performance impact of editor UI on rendering.
- Add unit tests for new editor classes.
- Document editor usage for developers.
- Consider packaging as separate executable or mode.

## Risks and Considerations
- SDL2 integration may require significant changes to Vulkan surface creation.
- Custom Slate-like UI framework needs to be integrated with Vulkan pipeline.
- Ensure editor doesn't break existing gameplay functionality.
- Performance: Editor UI should not significantly impact frame rates.
