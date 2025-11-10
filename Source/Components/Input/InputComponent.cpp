#include "Components/Input/InputComponent.h"

#include "Input/InputSystem.h"

// Добавим GLFW ключи для совместимости
#define GLFW_KEY_W 87
#define GLFW_KEY_S 83
#define GLFW_KEY_A 65
#define GLFW_KEY_D 68
#define GLFW_KEY_SPACE 32
#define GLFW_KEY_LEFT_SHIFT 340
#define GLFW_KEY_E 69
#define GLFW_KEY_ESCAPE 256

#define GLFW_PRESS 1
#define GLFW_RELEASE 0
#define GLFW_REPEAT 2

namespace CE
{
  InputComponent::InputComponent(CEObject* Owner, FString NewName)
      : CEComponent(Owner, NewName)
  {
    // Настраиваем маппинг клавиш по умолчанию
    SetupDefaultKeyMappings();
    CE_CORE_DEBUG("InputComponent created: ", NewName);
  }

  InputComponent::~InputComponent()
  {
    // Деструктор теперь определен здесь
    CE_CORE_DEBUG("InputComponent destroyed: ", GetName());
  }

  void InputComponent::BindAction(const FString& ActionName, EInputEvent EventType, std::function<void()> Callback)
  {
    m_ActionBindings[ActionName].push_back({Callback, EventType});
    CE_CORE_DEBUG("Bound action: ", ActionName, " event type: ", static_cast<int>(EventType));
  }

  void InputComponent::BindAxis(const FString& AxisName, std::function<void(float)> Callback, float Scale)
  {
    m_AxisBindings[AxisName] = {Callback, Scale};
    CE_CORE_DEBUG("Bound axis: ", AxisName, " scale: ", Scale);
  }

  void InputComponent::ProcessKey(int key, int action, [[maybe_unused]] float deltaTime)
  {
    // Определяем тип события
    EInputEvent eventType;
    if (action == GLFW_PRESS)
      eventType = EInputEvent::Pressed;
    else if (action == GLFW_RELEASE)
      eventType = EInputEvent::Released;
    else
      eventType = EInputEvent::Repeat;

    // Обрабатываем action биндинги
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

  void InputComponent::ProcessMouseMovement(float xOffset, float yOffset, [[maybe_unused]] float deltaTime)
  {
    // Обрабатываем ось LookHorizontal
    auto lookHorizontalIt = m_AxisBindings.find("LookHorizontal");
    if (lookHorizontalIt != m_AxisBindings.end())
    {
      lookHorizontalIt->second.Callback(xOffset);
    }

    // Обрабатываем ось LookVertical
    auto lookVerticalIt = m_AxisBindings.find("LookVertical");
    if (lookVerticalIt != m_AxisBindings.end())
    {
      lookVerticalIt->second.Callback(yOffset);
    }
  }

  void InputComponent::ProcessMouseScroll([[maybe_unused]] float yOffset)
  {
    // Можно добавить биндинг для скролла мыши
  }

  bool InputComponent::IsKeyPressed(int key) const
  {
    auto it = m_KeyStates.find(key);
    return it != m_KeyStates.end() && it->second;
  }

  void InputComponent::Update(float DeltaTime)
  {
    CEComponent::Update(DeltaTime);

    // Обрабатываем непрерывные axis биндинги для клавиш
    for (const auto& [key, axisName] : m_KeyToAxisMap)
    {
      if (IsKeyPressed(key))
      {
        auto axisIt = m_AxisBindings.find(axisName);
        if (axisIt != m_AxisBindings.end())
        {
          float value = 1.0f;

          // Определяем направление для осей движения
          if (axisName == "MoveForward")
          {
            value = (key == GLFW_KEY_W) ? 1.0f : -1.0f;
          }
          else if (axisName == "MoveRight")
          {
            value = (key == GLFW_KEY_D) ? 1.0f : -1.0f;
          }

          value *= axisIt->second.Scale;
          axisIt->second.Callback(value);
        }
      }
    }
  }

  void InputComponent::SetupDefaultKeyMappings()
  {
    // Маппинг клавиш на имена осей (для непрерывного ввода)
    m_KeyToAxisMap[GLFW_KEY_W] = "MoveForward";
    m_KeyToAxisMap[GLFW_KEY_S] = "MoveForward";
    m_KeyToAxisMap[GLFW_KEY_A] = "MoveRight";
    m_KeyToAxisMap[GLFW_KEY_D] = "MoveRight";

    // Маппинг клавиш на имена действий (для дискретных событий)
    m_KeyToActionMap[GLFW_KEY_SPACE] = "Jump";
    m_KeyToActionMap[GLFW_KEY_LEFT_SHIFT] = "Sprint";
    m_KeyToActionMap[GLFW_KEY_E] = "Interact";
    m_KeyToActionMap[GLFW_KEY_ESCAPE] = "Pause";

    CE_CORE_DEBUG("Default key mappings setup");
  }
}  // namespace CE