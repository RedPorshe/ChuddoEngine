# Phase 0: Create Custom Slate-like UI Framework

## Overview
This phase focuses on building a custom UI framework inspired by Unreal Engine's Slate, which is an immediate-mode GUI system. Slate provides a declarative way to define UI elements, handles layout, styling, and rendering efficiently. Key aspects include:

- **Immediate-Mode Rendering**: UI is rebuilt every frame based on current state, allowing dynamic changes.
- **Widget System**: Hierarchical widgets (e.g., buttons, panels, windows) with properties like position, size, and events.
- **Styling and Theming**: Support for colors, fonts, and skins.
- **Layout Management**: Automatic or manual layout of UI elements.
- **Event Handling**: Mouse/keyboard input, focus management.
- **Integration with Rendering**: Seamless integration with Vulkan for efficient GPU rendering.
- **Docking and Windows**: Support for dockable panels, resizable windows, and property editors.

## Tasks
- [ ] Create custom Slate-like UI framework for editor UI (docking, windows, property editors) - NOT using ready-made libraries like ImGui.
- [ ] Implement immediate-mode GUI rendering system similar to Unreal Engine's Slate.
- [ ] Implement basic UI components: windows, panels, buttons, text inputs, etc.
- [ ] Ensure UI rendering integrates with Vulkan pipeline.
- [ ] Add widget hierarchy and layout system (e.g., vertical/horizontal boxes, grids).
- [ ] Implement event handling for mouse clicks, keyboard input, and focus.
- [ ] Add styling system for colors, fonts, and themes.
- [ ] Create property editor widgets for editing actor/component properties.
- [ ] Implement docking system for resizable and movable panels.
- [ ] Add text rendering and input fields.
- [ ] Ensure performance: UI should render efficiently without impacting game frame rates.
- [ ] Integrate with custom math library (Vector2, Vector3, Matrix4, etc.) for UI positions, sizes, transformations, and layout calculations.

## Notes
- Remove all traces of ImGui from the project if any exist (though none were found in the current codebase).
- Build the framework from scratch without external UI libraries.
- This is not exhaustive; Unreal's Slate is highly complex. Start with basics and iterate.
- Reference Unreal's Slate documentation for inspiration, but adapt to C++ and Vulkan.

## Naming Conventions (Inspired by Unreal Engine)
Since the system is inspired by Unreal Engine (inheritance from CObject, like UObject in Unreal), follow these naming conventions:
1. For structs, prefix F (including math: in CoreMinimal.h, include CoreTypes.h, and define using CEMath::Vector2f = FVector2D; etc. Update all files using math to use new types from CoreTypes).
2. For enums, prefix E.
3. For interfaces, prefix I.
4. For main classes (object, actor, pawn, character, controller, etc.), prefix C (like U in Unreal).
- When writing C++ code, classes cannot be named without these prefixes.
