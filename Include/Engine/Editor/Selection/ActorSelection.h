#pragma once

#include <vector>
#include <Engine/Utils/Math/AllMath.h>


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

        bool IsGizmoHovered(const CEMath::Vector3f& rayOrigin, const CEMath::Vector3f& rayDirection, CCameraComponent* camera) const;
        bool IsGizmoSelected(const CEMath::Vector3f& rayOrigin, const CEMath::Vector3f& rayDirection, CCameraComponent* camera) const;
        void StartGizmoDrag(const CEMath::Vector3f& initialPosition);
        void UpdateGizmoDrag(const CEMath::Vector3f& currentPosition, const CEMath::Vector3f& dragDelta);
        void EndGizmoDrag();

        void RenderGizmo(CEMath::Matrix4f viewMatrix, CEMath::Matrix4f projectionMatrix);

    private:
        ActorSelection() = default;
        ~ActorSelection() = default;
        ActorSelection(const ActorSelection&) = delete;
        ActorSelection& operator=(const ActorSelection&) = delete;

        void RenderTranslateGizmo(const CEMath::Matrix4f& mvp);
        void RenderRotateGizmo(const CEMath::Matrix4f& mvp);
        void RenderScaleGizmo(const CEMath::Matrix4f& mvp);

        CEMath::Vector3f GetGizmoPosition() const;
        bool RayIntersectsGizmo(const CEMath::Vector3f& rayOrigin, const CEMath::Vector3f& rayDirection,
                               EGizmoAxis axis, float& distance) const;

        std::vector<CActor*> m_SelectedActors;
        EGizmoAxis m_GizmoAxis = EGizmoAxis::None;
        EGizmoMode m_GizmoMode = EGizmoMode::Translate;
        bool m_IsDragging = false;
        CEMath::Vector3f m_InitialDragPosition;
    };