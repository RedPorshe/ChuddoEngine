#pragma once

#include "Actors/Actor.h"

class CPawn;
class CHUD;
class CGameMode;

class CPlayerController : public CActor
    {
    CHUDDO_DECLARE_CLASS ( CPlayerController, CActor );

    public:
        CPlayerController ( CObject * inOwner = nullptr, const std::string & inDisplayName = "PlayerController" );
        virtual ~CPlayerController ();

        // ========== POSSESSION ==========
        virtual void Possess ( CPawn * PawnToPossess );
        virtual void Unpossess ();
        CPawn * GetPawn () const { return ControlledPawn; }
        bool IsPossessing () const { return ControlledPawn != nullptr; }

        // ========== HUD MANAGEMENT ==========
        virtual CHUD * SpawnHUD ( CHUD * InHUD );
        virtual void SetHUD ( CHUD * inHUD );
        CHUD * GetHUD () const { return m_HUD; }

        // ========== GAME MODE ==========
        void SetOwningGameMode ( CGameMode * inGameMode );
        CGameMode * GetOwningGameMode () const { return OwningGameMode; }

        // ========== INPUT ==========       
        virtual void ProcessPlayerInput ( float DeltaTime );
        bool IsInputEnabled () const { return bInputEnabled; }
        void SetInputEnabled ( bool bEnabled ) { bInputEnabled = bEnabled; }

        // ========== CAMERA ==========
        virtual FVector GetViewLocation () const;
        virtual FQuat GetViewRotation () const;

        // ========== ACTOR OVERRIDES ==========
        virtual void Tick ( float DeltaTime ) override;
        virtual void BeginPlay () override;
        virtual void EndPlay () override;

        //// ========== SERIALIZATION ==========
        //virtual void SerializeProperties ( rapidjson::Value & jsonValue, rapidjson::Document::AllocatorType & allocator ) const override;
        //virtual void DeserializeProperties ( const rapidjson::Value & jsonValue ) override;

    protected:
        CGameMode * OwningGameMode = nullptr;
        CPawn * ControlledPawn = nullptr;
        CHUD * m_HUD = nullptr;

        // Input state
        bool bInputEnabled = true;
        bool bShowMouseCursor = false;

        // View target
        CActor * ViewTarget = nullptr;
    };

REGISTER_CLASS_FACTORY ( CPlayerController );