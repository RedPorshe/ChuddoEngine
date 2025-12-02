#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <memory>

// Forward declarations
class CSlateRenderer;
class CSlateApplication;

// Base widget class
class CSlateWidget
{
public:
    CSlateWidget();
    virtual ~CSlateWidget() = default;

    virtual void Tick([[maybe_unused]] float DeltaTime) {}
    virtual void Paint() = 0;

    // Layout
    virtual FVector2D GetDesiredSize() const { return FVector2D(0, 0); }
    virtual void ArrangeChildren() {}

    // Event handling
    virtual bool OnMouseButtonDown([[maybe_unused]] const FVector2D& MousePos, [[maybe_unused]] EMouseButton Button) { return false; }
    virtual bool OnMouseButtonUp([[maybe_unused]] const FVector2D& MousePos, [[maybe_unused]] EMouseButton Button) { return false; }
    virtual bool OnMouseMove([[maybe_unused]] const FVector2D& MousePos) { return false; }
    virtual bool OnKeyDown([[maybe_unused]] EKey Key) { return false; }
    virtual bool OnKeyUp([[maybe_unused]] EKey Key) { return false; }

    // Properties
    FVector2D Position;
    FVector2D Size;
    EVisibility Visibility = EVisibility::Visible;

    // Hierarchy
    TSharedPtr<CSlateWidget> Parent;
    std::vector<TSharedPtr<CSlateWidget>> Children;

    void AddChild(TSharedPtr<CSlateWidget> Child);
    void RemoveChild(TSharedPtr<CSlateWidget> Child);
};

// Slate application manages the UI
class CSlateApplication
{
public:
    static CSlateApplication& Get();

    void Initialize(TSharedPtr<CSlateRenderer> InRenderer);
    void Shutdown();

    void Tick(float DeltaTime);
    void Paint();

    // Event handling - called by InputSystem
    void ProcessMouseButtonDown(const FVector2D& MousePos, EMouseButton Button);
    void ProcessMouseButtonUp(const FVector2D& MousePos, EMouseButton Button);
    void ProcessMouseMove(const FVector2D& MousePos);
    void ProcessKeyDown(EKey Key);
    void ProcessKeyUp(EKey Key);

    // Widget management
    void AddWidget(TSharedPtr<CSlateWidget> Widget);
    void RemoveWidget(TSharedPtr<CSlateWidget> Widget);

    // Input integration methods
    void SetInputEnabled(bool bEnabled) { bInputEnabled = bEnabled; }
    bool IsInputEnabled() const { return bInputEnabled; }

    // Focus management
    void SetFocusedWidget(TSharedPtr<CSlateWidget> Widget);
    TSharedPtr<CSlateWidget> GetFocusedWidget() const { return FocusedWidget; }

    TSharedPtr<CSlateRenderer> GetRenderer() const { return Renderer; }

private:
    CSlateApplication() = default;
    ~CSlateApplication() = default;

    TSharedPtr<CSlateRenderer> Renderer;
    std::vector<TSharedPtr<CSlateWidget>> RootWidgets;
    TSharedPtr<CSlateWidget> FocusedWidget;
    bool bInputEnabled = true;

    // Helper methods
    TSharedPtr<CSlateWidget> FindWidgetUnderMouse(const FVector2D& MousePos);
    bool RouteMouseEvent(TSharedPtr<CSlateWidget> Widget, const FVector2D& MousePos, EMouseButton Button, bool bIsDown);
    bool RouteKeyEvent(EKey Key, bool bIsDown);
};

// Renderer interface for drawing UI
class CSlateRenderer
{
public:
    virtual ~CSlateRenderer() = default;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual void DrawRect(const FVector2D& Position, const FVector2D& Size, const FLinearColor& Color) = 0;
    virtual void DrawText(const FString& Text, const FVector2D& Position, const FLinearColor& Color) = 0;
    // Add more drawing functions as needed
};

// Utility functions
namespace SlateUtils
{
    bool IsPointInRect(const FVector2D& Point, const FVector2D& RectPos, const FVector2D& RectSize);
}
