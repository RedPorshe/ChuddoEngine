#include "Engine/GamePlay/Components/InputComponent.h"

#include "Engine/GamePlay/Input/InputSystem.h"
#include <SDL3/SDL.h>




  CInputComponent::CInputComponent(CObject* Owner, FString NewName)
      : CComponent(Owner, NewName)
  {
    SetupDefaultKeyMappings();

    CInputSystem::Get().RegisterInputComponent(this);

    CORE_DEBUG("InputComponent created: ", NewName);
  }

  CInputComponent::~CInputComponent()
  {
    CInputSystem::Get().UnregisterInputComponent(this);

    CORE_DEBUG("InputComponent destroyed: ", GetName());
  }

  void CInputComponent::BindAction(const FString& ActionName, EInputEvent EventType, std::function<void()> Callback)
  {
    m_ActionBindings[ActionName].push_back({Callback, EventType});
  }

  void CInputComponent::BindAxis(const FString& AxisName, std::function<void(float)> Callback, float Scale)
  {
    m_AxisBindings[AxisName] = {Callback, Scale};
  }

  void CInputComponent::ProcessKey(int key, int action, [[maybe_unused]] float deltaTime)
  {
    EInputEvent eventType;
    if (action == SDL_EVENT_KEY_DOWN)
      eventType = EInputEvent::Pressed;
    else if (action == SDL_EVENT_KEY_UP)
      eventType = EInputEvent::Released;
    else
      eventType = EInputEvent::Repeat;

    auto actionIt = m_KeyToActionMap.find(key);
    if (actionIt != m_KeyToActionMap.end())
    {
      const FString& actionName = actionIt->second;

      auto bindingsIt = m_ActionBindings.find(actionName);
      if (bindingsIt != m_ActionBindings.end())
      {
        for (const auto& binding : bindingsIt->second)
        {
          if (binding.EventType == eventType)
          {
            binding.Callback();
          }
        }
      }
    }
  }

  void CInputComponent::ProcessMouseMovement(float xOffset, float yOffset, float deltaTime)
  {
    float scaledX = (xOffset * 10.1f) * deltaTime;
    float scaledY = (yOffset * 10.1f) * deltaTime;
    auto lookHorizontalIt = m_AxisBindings.find("LookHorizontal");
    if (lookHorizontalIt != m_AxisBindings.end())
    {
      lookHorizontalIt->second.Callback(scaledX);
    }

    auto lookVerticalIt = m_AxisBindings.find("LookVertical");
    if (lookVerticalIt != m_AxisBindings.end())
    {
      lookVerticalIt->second.Callback(scaledY);
    }
  }

  void CInputComponent::ProcessMouseScroll([[maybe_unused]] float yOffset)
  {
  }

  bool CInputComponent::IsKeyPressed(int key) const
  {
    return CInputSystem::Get().IsKeyPressed(key);
  }

  void CInputComponent::Update(float DeltaTime)
  {
    CComponent::Update(DeltaTime);

    for (const auto& [key, axisName] : m_KeyToAxisMap)
    {
      if (IsKeyPressed(key))
      {
        auto axisIt = m_AxisBindings.find(axisName);
        if (axisIt != m_AxisBindings.end())
        {
          float value = 1.0f;

          if (axisName == "MoveForward")
          {
            value = (key == SDLK_W) ? 1.0f : -1.0f;
          }
          else if (axisName == "MoveRight")
          {
            value = (key == SDLK_D) ? 1.0f : -1.0f;
          }

          value *= axisIt->second.Scale;

          axisIt->second.Callback(value);
        }
      }
    }
  }

  void CInputComponent::SetupDefaultKeyMappings()
  {
    m_KeyToAxisMap[SDLK_W] = "MoveForward";
    m_KeyToAxisMap[SDLK_S] = "MoveForward";
    m_KeyToAxisMap[SDLK_A] = "MoveRight";
    m_KeyToAxisMap[SDLK_D] = "MoveRight";

    m_KeyToActionMap[SDLK_SPACE] = "Jump";
    m_KeyToActionMap[SDLK_LSHIFT] = "Sprint";
    
    m_KeyToActionMap[SDLK_ESCAPE] = "Pause";

    CORE_DEBUG("Default key mappings setup");

    // Отладочный вывод маппинга
    for (const auto& [key, axis] : m_KeyToAxisMap)
    {
      INPUT_DEBUG("Axis mapping: key=", key, " -> ", axis);
    }
    for (const auto& [key, action] : m_KeyToActionMap)
    {
      INPUT_DEBUG("Action mapping: key=", key, " -> ", action);
    }
  }