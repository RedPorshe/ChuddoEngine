#pragma once

#include "Core/Object.h"

class CBaseComponent : public CObject
{
public:
	CBaseComponent(const CObject* Owner, const std::string& inName = "BaseComponent");
	virtual ~CBaseComponent();

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

	void SetActive(bool bNewActive) { bIsActive = bNewActive; }
	bool IsActive() const { return bIsActive; }
	void SetVisible(bool bNewVisible) { bisVisible = bNewVisible; }
	bool IsVisible() const { return bisVisible; }
	void SetCanTick(bool bNewCanTick) { bIsCanTick = bNewCanTick; }
	bool IsCanTick() const { return bIsCanTick; }

	void InitializeComponent();
private:
	bool bIsInitialized = false;
	bool bIsActive = true;
	bool bisVisible = true;
	bool bIsCanTick = true;

	// Component-specific methods can be added here
};