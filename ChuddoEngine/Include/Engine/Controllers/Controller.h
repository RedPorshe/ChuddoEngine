#pragma once

#include "Actors/Actor.h"

class CPawn;

class CController : public  CActor
{
	public:
	CController(const CObject* Owner, const std::string& inName = "Controller");
	explicit CController(const std::string& inName = "Controller");
	virtual ~CController();
	// Controller-specific virtual methods
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	CPawn* GetControlledPawn() const;
	void SetControlledPawn(CPawn* NewPawn); 
	
	bool Possess(CPawn* PawnToPossess);
	void UnPossess();
protected:
	CPawn* ControlledPawn = nullptr;
	std::string ControllerName;
};
