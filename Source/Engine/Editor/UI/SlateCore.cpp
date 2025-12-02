#include "Engine/Editor/UI/SlateCore.h"

CSlateWidget::CSlateWidget()
{
}

void CSlateWidget::AddChild(TSharedPtr<CSlateWidget> Child)
{
    if (Child)
    {
        Child->Parent = TSharedPtr<CSlateWidget>(this);
        Children.push_back(Child);
    }
}

void CSlateWidget::RemoveChild(TSharedPtr<CSlateWidget> Child)
{
    if (Child)
    {
        auto it = std::find(Children.begin(), Children.end(), Child);
        if (it != Children.end())
        {
            (*it)->Parent = nullptr;
            Children.erase(it);
        }
    }
}

CSlateApplication& CSlateApplication::Get()
{
    static CSlateApplication Instance;
    return Instance;
}

void CSlateApplication::Initialize(TSharedPtr<CSlateRenderer> InRenderer)
{
    Renderer = InRenderer;
}

void CSlateApplication::Shutdown()
{
    RootWidgets.clear();
    Renderer = nullptr;
}

void CSlateApplication::Tick(float DeltaTime)
{
    for (auto& Widget : RootWidgets)
    {
        Widget->Tick(DeltaTime);
    }
}

void CSlateApplication::Paint()
{
    if (Renderer)
    {
        Renderer->BeginFrame();
        for (auto& Widget : RootWidgets)
        {
            Widget->Paint();
        }
        Renderer->EndFrame();
    }
}

void CSlateApplication::ProcessMouseButtonDown(const FVector2D& MousePos, EMouseButton Button)
{
    if (!bInputEnabled) return;

    auto WidgetUnderMouse = FindWidgetUnderMouse(MousePos);
    if (WidgetUnderMouse)
    {
        RouteMouseEvent(WidgetUnderMouse, MousePos, Button, true);
        SetFocusedWidget(WidgetUnderMouse);
    }
}

void CSlateApplication::ProcessMouseButtonUp(const FVector2D& MousePos, EMouseButton Button)
{
    if (!bInputEnabled) return;

    auto WidgetUnderMouse = FindWidgetUnderMouse(MousePos);
    if (WidgetUnderMouse)
    {
        RouteMouseEvent(WidgetUnderMouse, MousePos, Button, false);
    }
}

void CSlateApplication::ProcessMouseMove(const FVector2D& MousePos)
{
    if (!bInputEnabled) return;

    auto WidgetUnderMouse = FindWidgetUnderMouse(MousePos);
    if (WidgetUnderMouse)
    {
        WidgetUnderMouse->OnMouseMove(MousePos);
    }
}

void CSlateApplication::ProcessKeyDown(EKey Key)
{
    if (!bInputEnabled) return;

    RouteKeyEvent(Key, true);
}

void CSlateApplication::ProcessKeyUp(EKey Key)
{
    if (!bInputEnabled) return;

    RouteKeyEvent(Key, false);
}

void CSlateApplication::AddWidget(TSharedPtr<CSlateWidget> Widget)
{
    if (Widget)
    {
        RootWidgets.push_back(Widget);
    }
}

void CSlateApplication::RemoveWidget(TSharedPtr<CSlateWidget> Widget)
{
    if (Widget)
    {
        auto it = std::find(RootWidgets.begin(), RootWidgets.end(), Widget);
        if (it != RootWidgets.end())
        {
            RootWidgets.erase(it);
        }
    }
}

void CSlateApplication::SetFocusedWidget(TSharedPtr<CSlateWidget> Widget)
{
    FocusedWidget = Widget;
}

TSharedPtr<CSlateWidget> CSlateApplication::FindWidgetUnderMouse(const FVector2D& MousePos)
{
    // Search from front to back (top widgets first)
    for (auto it = RootWidgets.rbegin(); it != RootWidgets.rend(); ++it)
    {
        auto& widget = *it;
        if (widget->Visibility == EVisibility::Visible &&
            SlateUtils::IsPointInRect(MousePos, widget->Position, widget->Size))
        {
            return widget;
        }
    }
    return nullptr;
}

bool CSlateApplication::RouteMouseEvent(TSharedPtr<CSlateWidget> Widget, const FVector2D& MousePos, EMouseButton Button, bool bIsDown)
{
    if (!Widget) return false;

    if (bIsDown)
    {
        return Widget->OnMouseButtonDown(MousePos, Button);
    }
    else
    {
        return Widget->OnMouseButtonUp(MousePos, Button);
    }
}

bool CSlateApplication::RouteKeyEvent(EKey Key, bool bIsDown)
{
    if (!FocusedWidget) return false;

    if (bIsDown)
    {
        return FocusedWidget->OnKeyDown(Key);
    }
    else
    {
        return FocusedWidget->OnKeyUp(Key);
    }
}

bool SlateUtils::IsPointInRect(const FVector2D& Point, const FVector2D& RectPos, const FVector2D& RectSize)
{
    return Point.x >= RectPos.x && Point.x <= RectPos.x + RectSize.x &&
           Point.y >= RectPos.y && Point.y <= RectPos.y + RectSize.y;
}
