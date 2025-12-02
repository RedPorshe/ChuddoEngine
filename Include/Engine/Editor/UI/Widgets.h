#pragma once

#include "Engine/Editor/UI/SlateCore.h"

// Basic widgets
class CButton : public CSlateWidget
{
public:
    CButton(const FString& InText = "");

    virtual void Paint() override;
    virtual FVector2D GetDesiredSize() const override;

    virtual bool OnMouseButtonDown([[maybe_unused]] const FVector2D& MousePos, [[maybe_unused]] EMouseButton Button) override;
    virtual bool OnMouseButtonUp([[maybe_unused]] const FVector2D& MousePos, [[maybe_unused]] EMouseButton Button) override;

    // Events
    std::function<void()> OnClicked;

    FString Text;
    FLinearColor NormalColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
    FLinearColor HoveredColor = FLinearColor(0.7f, 0.7f, 0.7f, 1.0f);
    FLinearColor PressedColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);

private:
    bool bIsHovered = false;
    bool bIsPressed = false;
};

class CTextBlock : public CSlateWidget
{
public:
    CTextBlock(const FString& InText = "");

    virtual void Paint() override;
    virtual FVector2D GetDesiredSize() const override;

    FString Text;
    FLinearColor Color = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
};

class CPanel : public CSlateWidget
{
public:
    CPanel();

    virtual void Paint() override;
    virtual void ArrangeChildren() override;
    virtual FVector2D GetDesiredSize() const override;

    FLinearColor BackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
};

// Layout widgets
class CVerticalBox : public CPanel
{
public:
    CVerticalBox();

    virtual void ArrangeChildren() override;
    virtual FVector2D GetDesiredSize() const override;
};

class CHorizontalBox : public CPanel
{
public:
    CHorizontalBox();

    virtual void ArrangeChildren() override;
    virtual FVector2D GetDesiredSize() const override;
};

// Window widget
class CWindow : public CPanel
{
public:
    CWindow(const FString& InTitle = "");

    virtual void Paint() override;
    virtual void ArrangeChildren() override;
    virtual FVector2D GetDesiredSize() const override;

    virtual bool OnMouseButtonDown([[maybe_unused]] const FVector2D& MousePos, [[maybe_unused]] EMouseButton Button) override;
    virtual bool OnMouseMove([[maybe_unused]] const FVector2D& MousePos) override;

    FString Title;
    bool bIsDraggable = true;
    bool bIsResizable = true;

private:
    bool bIsDragging = false;
    FVector2D DragOffset;
};
