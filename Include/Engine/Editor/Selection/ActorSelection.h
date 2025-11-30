#pragma once

#include <vector>
#include <Engine/Utils/Math/AllMath.h>

namespace CE
{
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

        bool IsGizmoHovered(const CE::Math::Vector3f& rayOrigin, const CE::Math::Vector3f& rayDirection, CCameraComponent* camera) const;
        bool IsGizmoSelected(const CE::Math::Vector3f& rayOrigin, const CE::Math::Vector3f& rayDirection, CCameraComponent* camera) const;
        void StartGizmoDrag(const CE::Math::Vector3f& initialPosition);
        void UpdateGizmoDrag(const CE::Math::Vector3f& currentPosition, const CE::Math::Vector3f& dragDelta);
        void EndGizmoDrag();

        void RenderGizmo(CE::Math::Matrix4f viewMatrix, CE::Math::Matrix4f projectionMatrix);

    private:
        ActorSelection() = default;
        ~ActorSelection() = default;
        ActorSelection(const ActorSelection&) = delete;
        ActorSelection& operator=(const ActorSelection&) = delete;

        void RenderTranslateGizmo(const CE::Math::Matrix4f& mvp);
        void RenderRotateGizmo(const CE::Math::Matrix4f& mvp);
        void RenderScaleGizmo(const CE::Math::Matrix4f& mvp);

        CE::Math::Vector3f GetGizmoPosition() const;
        bool RayIntersectsGizmo(const CE::Math::Vector3f& rayOrigin, const CE::Math::Vector3f& rayDirection,
                               EGizmoAxis axis, float& distance) const;

        std::vector<CActor*> m_SelectedActors;
        EGizmoAxis m_GizmoAxis = EGizmoAxis::None;
        EGizmoMode m_GizmoMode = EGizmoMode::Translate;
        bool m_IsDragging = false;
        CE::Math::Vector3f m_InitialDragPosition;
    };
} // namespace CE
