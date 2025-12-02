#pragma once

#include "Engine/GamePlay/Components/InputComponent.h"
#include "Engine/Editor/UI/SlateCore.h"

/**
 * Input component that bridges InputSystem events to SlateApplication
 */
class CSlateInputComponent : public CInputComponent
{
public:
    CSlateInputComponent();
    virtual ~CSlateInputComponent();

    virtual void ProcessKey(int key, int action, float deltaTime) override;
    virtual void ProcessMouseMovement(float xoffset, float yoffset, float deltaTime) override;
    virtual void ProcessMouseScroll(float yoffset) override;

private:
    EKey ConvertSDLKeyToSlateKey(int sdlKey) const;
    EMouseButton ConvertSDLButtonToSlateButton(int sdlButton) const;
};
