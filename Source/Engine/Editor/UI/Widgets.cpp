#include "Engine/Editor/UI/Widgets.h"


// CButton implementation
CButton::CButton(const FString& InText)
    : Text(InText)
{
}

void CButton::Paint()
{
    if (auto Renderer = CSlateApplication::Get().GetRenderer())
    {
        FLinearColor Color = bIsPressed ? PressedColor : (bIsHovered ? HoveredColor : NormalColor);
        Renderer->DrawRect(Position, Size, Color);
        // Draw text centered
        FVector2D TextPos = Position + FVector2D(Size.x * 0.5f, Size.y * 0.5f);
        Renderer->DrawText(Text, TextPos, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
    }
}

FVector2D CButton::GetDesiredSize() const
{
    // Simple size calculation - in real implementation, would measure text
    return FVector2D(100.0f, 30.0f);
}

bool CButton::OnMouseButtonDown(const FVector2D& MousePos, EMouseButton Button)
{
    if (Button == EMouseButton::Left && SlateUtils::IsPointInRect(MousePos, Position, Size))
    {
        bIsPressed = true;
        return true;
    }
    return false;
}

bool CButton::OnMouseButtonUp(const FVector2D& MousePos, EMouseButton Button)
{
    if (Button == EMouseButton::Left && bIsPressed)
    {
        bIsPressed = false;
        if (SlateUtils::IsPointInRect(MousePos, Position, Size) && OnClicked)
        {
            OnClicked();
        }
        return true;
    }
    return false;
}

// CTextBlock implementation
CTextBlock::CTextBlock(const FString& InText)
    : Text(InText)
{
}

void CTextBlock::Paint()
{
    if (auto Renderer = CSlateApplication::Get().GetRenderer())
    {
        Renderer->DrawText(Text, Position, Color);
    }
}

FVector2D CTextBlock::GetDesiredSize() const
{
    // Simple size calculation - in real implementation, would measure text
    return FVector2D(static_cast<float>(Text.length()) * 10.0f, 20.0f);
}

// CPanel implementation
CPanel::CPanel()
{
}

void CPanel::Paint()
{
    if (auto Renderer = CSlateApplication::Get().GetRenderer())
    {
        Renderer->DrawRect(Position, Size, BackgroundColor);
    }
}

void CPanel::ArrangeChildren()
{
    // Base implementation - no arrangement
}

FVector2D CPanel::GetDesiredSize() const
{
    // Return size of children bounds
    FVector2D MinPos(FLT_MAX, FLT_MAX);
    FVector2D MaxPos(-FLT_MAX, -FLT_MAX);

    for (const auto& Child : Children)
    {
        MinPos = FVector2D(std::min(MinPos.x, Child->Position.x), std::min(MinPos.y, Child->Position.y));
        MaxPos = FVector2D(std::max(MaxPos.x, Child->Position.x + Child->Size.x), std::max(MaxPos.y, Child->Position.y + Child->Size.y));
    }

    return MaxPos - MinPos;
}

// CVerticalBox implementation
CVerticalBox::CVerticalBox()
{
}

void CVerticalBox::ArrangeChildren()
{
    float CurrentY = Position.y;
    for (const auto& Child : Children)
    {
        Child->Position = FVector2D(Position.x, CurrentY);
        Child->Size.x = Size.x;
        CurrentY += Child->GetDesiredSize().y;
    }
}

FVector2D CVerticalBox::GetDesiredSize() const
{
    FVector2D TotalSize(0.0f, 0.0f);
    for (const auto& Child : Children)
    {
        FVector2D ChildSize = Child->GetDesiredSize();
        TotalSize.x = std::max(TotalSize.x, ChildSize.x);
        TotalSize.y += ChildSize.y;
    }
    return TotalSize;
}

// CHorizontalBox implementation
CHorizontalBox::CHorizontalBox()
{
}

void CHorizontalBox::ArrangeChildren()
{
    float CurrentX = Position.x;
    for (const auto& Child : Children)
    {
        Child->Position = FVector2D(CurrentX, Position.y);
        Child->Size.y = Size.y;
        CurrentX += Child->GetDesiredSize().x;
    }
}

FVector2D CHorizontalBox::GetDesiredSize() const
{
    FVector2D TotalSize(0.0f, 0.0f);
    for (const auto& Child : Children)
    {
        FVector2D ChildSize = Child->GetDesiredSize();
        TotalSize.x += ChildSize.x;
        TotalSize.y = std::max(TotalSize.y, ChildSize.y);
    }
    return TotalSize;
}

// CWindow implementation
CWindow::CWindow(const FString& InTitle)
    : Title(InTitle)
{
}

void CWindow::Paint()
{
    if (auto Renderer = CSlateApplication::Get().GetRenderer())
    {
        // Draw window background
        Renderer->DrawRect(Position, Size, FLinearColor(0.8f, 0.8f, 0.8f, 1.0f));
        // Draw title bar
        Renderer->DrawRect(Position, FVector2D(Size.x, 30.0f), FLinearColor(0.6f, 0.6f, 0.6f, 1.0f));
        // Draw title text
        Renderer->DrawText(Title, Position + FVector2D(10.0f, 5.0f), FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
    }
}

void CWindow::ArrangeChildren()
{
    // Arrange children below title bar
    FVector2D ContentPos = Position + FVector2D(0.0f, 30.0f);
    FVector2D ContentSize = Size - FVector2D(0.0f, 30.0f);

    for (const auto& Child : Children)
    {
        Child->Position = ContentPos;
        Child->Size = ContentSize;
    }
}

FVector2D CWindow::GetDesiredSize() const
{
    // Include title bar height
    FVector2D ContentSize = CPanel::GetDesiredSize();
    return FVector2D(ContentSize.x, ContentSize.y + 30.0f);
}

bool CWindow::OnMouseButtonDown(const FVector2D& MousePos, EMouseButton Button)
{
    if (Button == EMouseButton::Left && bIsDraggable)
    {
        FVector2D TitleBarPos = Position;
        FVector2D TitleBarSize = FVector2D(Size.x, 30.0f);
        if (SlateUtils::IsPointInRect(MousePos, TitleBarPos, TitleBarSize))
        {
            bIsDragging = true;
            DragOffset = MousePos - Position;
            return true;
        }
    }
    return CSlateWidget::OnMouseButtonDown(MousePos, Button);
}

bool CWindow::OnMouseMove(const FVector2D& MousePos)
{
    if (bIsDragging)
    {
        Position = MousePos - DragOffset;
        return true;
    }
    return CSlateWidget::OnMouseMove(MousePos);
}
