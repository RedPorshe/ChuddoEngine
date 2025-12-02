#pragma once

#include <functional>
#include <unordered_map>

#include "Engine/GamePlay/Components/Base/Component.h"



  enum class EInputEvent
  {
    Pressed,
    Released,
    Repeat,
    Axis
  };

  struct FInputActionBinding
  {
    std::function<void()> Callback;
    EInputEvent EventType;
  };

  struct InputAxisBinding
  {
    std::function<void(float)> Callback;
    float Scale = 1.0f;
  };

  class CInputComponent : public CComponent
  {
   public:
    CInputComponent(CObject* Owner = nullptr, FString NewName = "InputComponent");
    virtual ~CInputComponent();

    void BindAction(const FString& ActionName, EInputEvent EventType, std::function<void()> Callback);

    void BindAxis(const FString& AxisName, std::function<void(float)> Callback, float Scale = 1.0f);

    virtual void ProcessKey(int key, int action, float deltaTime);
    virtual void ProcessMouseMovement(float xOffset, float yOffset, float deltaTime);
    virtual void ProcessMouseScroll(float yOffset);

    bool IsKeyPressed(int key) const;

    void Update(float DeltaTime) override;

   private:
    std::unordered_map<FString, std::vector<FInputActionBinding>> m_ActionBindings;
    std::unordered_map<FString, InputAxisBinding> m_AxisBindings;
    std::unordered_map<int, bool> m_KeyStates;

    std::unordered_map<int, FString> m_KeyToActionMap;
    std::unordered_map<int, FString> m_KeyToAxisMap;

    float m_LastMouseX = 0.0f;
    float m_LastMouseY = 0.0f;
    bool m_FirstMouse = true;

    void SetupDefaultKeyMappings();
  };