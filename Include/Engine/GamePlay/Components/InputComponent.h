#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL.h>
#include "Engine/GamePlay/Components/Base/Component.h"
#include "Engine/Core/CoreTypes.h"
#include "Engine/GamePlay/Input/InputSystem.h"

class CInputComponent : public CComponent
{
public:
    CInputComponent(CObject* Owner = nullptr, FString NewName = "InputComponent");
    virtual ~CInputComponent();

    // Action bindings
    void BindAction(const FString& ActionName, EInputEvent EventType, std::function<void()> Callback);

    // Axis bindings
    void BindAxis(const FString& AxisName, std::function<void(float)> Callback, float Scale = 1.0f);

    // Trigger methods called by InputSystem
    void TriggerAction(const FString& ActionName, EInputEvent EventType);
    void TriggerAxis(const FString& AxisName, float Value);

    // State queries
    bool IsKeyDown(SDL_Keycode Key) const;
    bool IsMouseButtonDown(Uint8 Button) const;
    FVector2D GetMousePosition() const;
    FVector2D GetMouseDelta() const;

    // Update
    virtual void Update(float DeltaTime) override;

    // Setup default mappings
    virtual void SetupDefaultMappings();

protected:
    // Bindings
    std::unordered_map<FString, std::vector<FInputActionBinding>> m_ActionBindings;
    std::unordered_map<FString, FInputAxisBinding> m_AxisBindings;

    // Sensitivity
    float m_MouseSensitivity = 0.1f;
};
