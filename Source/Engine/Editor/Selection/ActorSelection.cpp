#include "Engine/Editor/Selection/ActorSelection.h"

#include "Engine/GamePlay/Actors/Actor.h"
#include "Engine/GamePlay/Components/CameraComponent.h"
#include "Engine/Core/Rendering/Vulkan/Core/VulkanContext.h"
#include "Engine/Core/CoreTypes.h"


  ActorSelection& ActorSelection::Get()
  {
    static ActorSelection instance;
    return instance;
  }

  void ActorSelection::SelectActor(CActor* actor)
  {
    if (!actor || IsSelected(actor)) return;

    m_SelectedActors.push_back(actor);
    CORE_DEBUG("Selected actor: ", actor->GetName().c_str());
  }

  void ActorSelection::DeselectActor(CActor* actor)
  {
    if (!actor) return;

    auto it = std::find(m_SelectedActors.begin(), m_SelectedActors.end(), actor);
    if (it != m_SelectedActors.end())
    {
      m_SelectedActors.erase(it);
      CORE_DEBUG("Deselected actor: ", actor->GetName().c_str());
    }
  }

  void ActorSelection::ClearSelection()
  {
    m_SelectedActors.clear();
    m_GizmoAxis = EGizmoAxis::None;
    CORE_DEBUG("Cleared actor selection");
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

  bool ActorSelection::IsGizmoHovered(const FVector& rayOrigin, const FVector& rayDirection, CCameraComponent* camera) const
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

  bool ActorSelection::IsGizmoSelected(const FVector& rayOrigin, const FVector& rayDirection, CCameraComponent* camera) const
  {
    if (m_SelectedActors.empty()) return false;

    float distance;
    return RayIntersectsGizmo(rayOrigin, rayDirection, m_GizmoAxis, distance);
  }

  void ActorSelection::StartGizmoDrag(const FVector& initialPosition)
  {
    m_IsDragging = true;
    m_InitialDragPosition = initialPosition;
    CORE_DEBUG("Started gizmo drag");
  }

  void ActorSelection::UpdateGizmoDrag(const FVector& currentPosition, const FVector& dragDelta)
  {
    if (!m_IsDragging || m_SelectedActors.empty()) return;

    // Apply transformation to selected actors
    for (CActor* actor : m_SelectedActors)
    {
      if (!actor) continue;

      CEMath::Vector3f newPosition = actor->GetActorLocation();

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
    CORE_DEBUG("Ended gizmo drag");
  }

  void ActorSelection::RenderGizmo(FMatrix viewMatrix, FMatrix projectionMatrix)
  {
    if (m_SelectedActors.empty()) return;

    FVector gizmoPos = GetGizmoPosition();
    FMatrix modelMatrix = FMatrix::Translation(gizmoPos);
    FMatrix mvp = projectionMatrix * viewMatrix * modelMatrix;

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

  void ActorSelection::RenderTranslateGizmo(const FMatrix& mvp)
  {
    // TODO: Implement gizmo rendering using Vulkan
    // This would involve creating vertex buffers for the gizmo geometry
    // and rendering them with appropriate colors based on selection state
  }

  void ActorSelection::RenderRotateGizmo(const FMatrix& mvp)
  {
    // TODO: Implement rotation gizmo rendering
  }

  void ActorSelection::RenderScaleGizmo(const FMatrix& mvp)
  {
    // TODO: Implement scale gizmo rendering
  }

  FVector ActorSelection::GetGizmoPosition() const
  {
    if (m_SelectedActors.empty()) return FVector::Zero();

    // For now, use the position of the primary selected actor
    CActor* primary = GetPrimarySelectedActor();
    return primary ? primary->GetActorLocation() : CEMath::Vector3f::Zero();
  }

  bool ActorSelection::RayIntersectsGizmo(const FVector& rayOrigin, const FVector& rayDirection,
                                         EGizmoAxis axis, float& distance) const
  {
    // TODO: Implement ray-gizmo intersection tests
    // This involves checking if the ray intersects with the gizmo geometry
    // (cylinders for translate, tori for rotate, etc.)

    distance = 0.0f;
    return false;  // Placeholder
  }