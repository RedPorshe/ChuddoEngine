#include "Engine/Editor/Selection/ActorSelection.h"

#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/Core/Rendering/Vulkan/Core/VulkanContext.h"

namespace CE
{
  ActorSelection& ActorSelection::Get()
  {
    static ActorSelection instance;
    return instance;
  }

  void ActorSelection::SelectActor(CActor* actor)
  {
    if (!actor || IsSelected(actor)) return;

    m_SelectedActors.push_back(actor);
    CE_CORE_DEBUG("Selected actor: ", actor->GetName().c_str());
  }

  void ActorSelection::DeselectActor(CActor* actor)
  {
    if (!actor) return;

    auto it = std::find(m_SelectedActors.begin(), m_SelectedActors.end(), actor);
    if (it != m_SelectedActors.end())
    {
      m_SelectedActors.erase(it);
      CE_CORE_DEBUG("Deselected actor: ", actor->GetName().c_str());
    }
  }

  void ActorSelection::ClearSelection()
  {
    m_SelectedActors.clear();
    m_GizmoAxis = EGizmoAxis::None;
    CE_CORE_DEBUG("Cleared actor selection");
  }

  void ActorSelection::ToggleSelection(CActor* actor)
  {
    if (IsSelected(actor))
    {
      DeselectActor(actor);
    }
    else
    {
      SelectActor(actor);
    }
  }

  bool ActorSelection::IsSelected(CActor* actor) const
  {
    return std::find(m_SelectedActors.begin(), m_SelectedActors.end(), actor) != m_SelectedActors.end();
  }

  CActor* ActorSelection::GetPrimarySelectedActor() const
  {
    return m_SelectedActors.empty() ? nullptr : m_SelectedActors.front();
  }

  bool ActorSelection::IsGizmoHovered(const CE::Math::Vector3f& rayOrigin, const CE::Math::Vector3f& rayDirection, CCameraComponent* camera) const
  {
    if (m_SelectedActors.empty()) return false;

    float minDistance = FLT_MAX;
    EGizmoAxis closestAxis = EGizmoAxis::None;

    // Check intersection with each gizmo axis
    for (int axis = static_cast<int>(EGizmoAxis::X); axis <= static_cast<int>(EGizmoAxis::Z); ++axis)
    {
      float distance;
      if (RayIntersectsGizmo(rayOrigin, rayDirection, static_cast<EGizmoAxis>(axis), distance))
      {
        if (distance < minDistance)
        {
          minDistance = distance;
          closestAxis = static_cast<EGizmoAxis>(axis);
        }
      }
    }

    // TODO: Check for plane intersections (XY, XZ, YZ)

    return closestAxis != EGizmoAxis::None;
  }

  bool ActorSelection::IsGizmoSelected(const CE::Math::Vector3f& rayOrigin, const CE::Math::Vector3f& rayDirection, CCameraComponent* camera) const
  {
    if (m_SelectedActors.empty()) return false;

    float distance;
    return RayIntersectsGizmo(rayOrigin, rayDirection, m_GizmoAxis, distance);
  }

  void ActorSelection::StartGizmoDrag(const CE::Math::Vector3f& initialPosition)
  {
    m_IsDragging = true;
    m_InitialDragPosition = initialPosition;
    CE_CORE_DEBUG("Started gizmo drag");
  }

  void ActorSelection::UpdateGizmoDrag(const CE::Math::Vector3f& currentPosition, const CE::Math::Vector3f& dragDelta)
  {
    if (!m_IsDragging || m_SelectedActors.empty()) return;

    // Apply transformation to selected actors
    for (CActor* actor : m_SelectedActors)
    {
      if (!actor) continue;

      CE::Math::Vector3f newPosition = actor->GetActorLocation();

      switch (m_GizmoMode)
      {
        case EGizmoMode::Translate:
          switch (m_GizmoAxis)
          {
            case EGizmoAxis::X:
              newPosition.x += dragDelta.x;
              break;
            case EGizmoAxis::Y:
              newPosition.y += dragDelta.y;
              break;
            case EGizmoAxis::Z:
              newPosition.z += dragDelta.z;
              break;
            default:
              break;
          }
          actor->SetActorLocation(newPosition);
          break;

        case EGizmoMode::Rotate:
          // TODO: Implement rotation
          break;

        case EGizmoMode::Scale:
          // TODO: Implement scaling
          break;

        default:
          break;
      }
    }
  }

  void ActorSelection::EndGizmoDrag()
  {
    m_IsDragging = false;
    CE_CORE_DEBUG("Ended gizmo drag");
  }

  void ActorSelection::RenderGizmo(CE::Math::Matrix4f viewMatrix, CE::Math::Matrix4f projectionMatrix)
  {
    if (m_SelectedActors.empty()) return;

    CE::Math::Vector3f gizmoPos = GetGizmoPosition();
    CE::Math::Matrix4f modelMatrix = CE::Math::Matrix4f::Translation(gizmoPos);
    CE::Math::Matrix4f mvp = projectionMatrix * viewMatrix * modelMatrix;

    switch (m_GizmoMode)
    {
      case EGizmoMode::Translate:
        RenderTranslateGizmo(mvp);
        break;
      case EGizmoMode::Rotate:
        RenderRotateGizmo(mvp);
        break;
      case EGizmoMode::Scale:
        RenderScaleGizmo(mvp);
        break;
      default:
        break;
    }
  }

  void ActorSelection::RenderTranslateGizmo(const CE::Math::Matrix4f& mvp)
  {
    // TODO: Implement gizmo rendering using Vulkan
    // This would involve creating vertex buffers for the gizmo geometry
    // and rendering them with appropriate colors based on selection state
  }

  void ActorSelection::RenderRotateGizmo(const CE::Math::Matrix4f& mvp)
  {
    // TODO: Implement rotation gizmo rendering
  }

  void ActorSelection::RenderScaleGizmo(const CE::Math::Matrix4f& mvp)
  {
    // TODO: Implement scale gizmo rendering
  }

  CE::Math::Vector3f ActorSelection::GetGizmoPosition() const
  {
    if (m_SelectedActors.empty()) return CE::Math::Vector3f::Zero();

    // For now, use the position of the primary selected actor
    CActor* primary = GetPrimarySelectedActor();
    return primary ? primary->GetActorLocation() : CE::Math::Vector3f::Zero();
  }

  bool ActorSelection::RayIntersectsGizmo(const CE::Math::Vector3f& rayOrigin, const CE::Math::Vector3f& rayDirection,
                                         EGizmoAxis axis, float& distance) const
  {
    // TODO: Implement ray-gizmo intersection tests
    // This involves checking if the ray intersects with the gizmo geometry
    // (cylinders for translate, tori for rotate, etc.)

    distance = 0.0f;
    return false;  // Placeholder
  }
}  // namespace CE
