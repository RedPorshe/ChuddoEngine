#pragma once

#include <functional>
#include <unordered_map>

#include "Components/Base/Component.h"

namespace CE
{
  // Типы биндингов как в UE
  enum class EInputEvent
  {
    Pressed,
    Released,
    Repeat,
    Axis
  };

  // Биндинг для действий (Pressed/Released)
  struct InputActionBinding
  {
    std::function<void()> Callback;
    EInputEvent EventType;
  };

  // Биндинг для осей (непрерывный ввод)
  struct InputAxisBinding
  {
    std::function<void(float)> Callback;
    float Scale = 1.0f;
  };

  class InputComponent : public CEComponent
  {
   public:
    InputComponent(CEObject* Owner = nullptr, FString NewName = "InputComponent");
    virtual ~InputComponent();  // Убрали = default

    // Биндинг действий (как в UE)
    void BindAction(const FString& ActionName, EInputEvent EventType, std::function<void()> Callback);

    // Биндинг осей (как в UE)
    void BindAxis(const FString& AxisName, std::function<void(float)> Callback, float Scale = 1.0f);

    // Обработка ввода из Application
    void ProcessKey(int key, int action, float deltaTime);
    void ProcessMouseMovement(float xOffset, float yOffset, float deltaTime);
    void ProcessMouseScroll(float yOffset);

    // Состояние ввода
    bool IsKeyPressed(int key) const;

    // Обновление состояний (вызывается каждый кадр)
    void Update(float DeltaTime) override;

   private:
    std::unordered_map<FString, std::vector<InputActionBinding>> m_ActionBindings;
    std::unordered_map<FString, InputAxisBinding> m_AxisBindings;
    std::unordered_map<int, bool> m_KeyStates;

    // Маппинг клавиш на имена действий/осей
    std::unordered_map<int, FString> m_KeyToActionMap;
    std::unordered_map<int, FString> m_KeyToAxisMap;

    // Состояние мыши
    float m_LastMouseX = 0.0f;
    float m_LastMouseY = 0.0f;
    bool m_FirstMouse = true;

    void SetupDefaultKeyMappings();
  };
}  // namespace CE