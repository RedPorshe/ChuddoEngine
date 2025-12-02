#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "Engine/Core/CoreTypes.h"

    class CActor;
    class AActor;
    class CCameraComponent;

    enum class EGizmoAxis
    {
        None,
        X,
        Y,
        Z
    };

    enum class EGizmoMode
    {
        Translate,
        Rotate,
        Scale
    };

    class ActorSelection
    {
    public:
        static ActorSelection& Get();

        void SelectActor(CActor* actor);
        void DeselectActor(CActor* actor);
        void ClearSelection();
    void ToggleSelection(CActor* actor);
        bool IsSelected(CActor* actor) const;
        CActor* GetPrimarySelectedActor() const;

        bool IsGizmoHovered(const FVector& rayOrigin, const FVector& rayDirection, CCameraComponent* camera) const;
        bool IsGizmoSelected(const FVector& rayOrigin, const FVector& rayDirection, CCameraComponent* camera) const;
        void StartGizmoDrag(const FVector& initialPosition);
        void UpdateGizmoDrag(const FVector& currentPosition, const FVector& dragDelta);
        void EndGizmoDrag();

        void RenderGizmo(FMatrix viewMatrix, FMatrix projectionMatrix);

    private:
        ActorSelection() = default;
        ~ActorSelection() = default;
        ActorSelection(const ActorSelection&) = delete;
        ActorSelection& operator=(const ActorSelection&) = delete;

        void RenderTranslateGizmo(const FMatrix& mvp);
        void RenderRotateGizmo(const FMatrix& mvp);
        void RenderScaleGizmo(const FMatrix& mvp);

        FVector GetGizmoPosition() const;
        bool RayIntersectsGizmo(const FVector& rayOrigin, const FVector& rayDirection,
                               EGizmoAxis axis, float& distance) const;

        std::vector<CActor*> m_SelectedActors;
        EGizmoAxis m_GizmoAxis = EGizmoAxis::None;
        EGizmoMode m_GizmoMode = EGizmoMode::Translate;
        bool m_IsDragging = false;
        FVector m_InitialDragPosition;
    };