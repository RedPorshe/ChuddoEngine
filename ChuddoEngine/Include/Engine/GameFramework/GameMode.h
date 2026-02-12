#pragma once

#include "Core/Object.h"
#include "GameModeSettings.h"

// Forward declarations
class CWorld;
class CLevel;
class CPawn;
class CPlayerController;
class CHUD;
class CActor;
class CPlayerStart;




class CGameMode : public CObject
    {
    CHUDDO_DECLARE_CLASS ( CGameMode, CObject )

    public:
        CGameMode ( CObject * inOwner = nullptr, const std::string & inName = "GameMode" );
        virtual ~CGameMode ();

        // ========== LIFECYCLE ==========
        virtual void InitGame ();
        virtual void StartPlay ();
        virtual void Tick ( float DeltaTime );
        virtual void EndGame ();

        // ========== CLASS SETTINGS ==========
        void SetDefaultPawnClass ( const std::string & className );
        void SetDefaultPlayerControllerClass ( const std::string & className ) { Settings->SetPlayerController ( className ); }
        void SetDefaultHUDClass ( const std::string & className ) { Settings->SetHUD ( className ); }
        void SetDefaultPlayerStartClass ( const std::string & className ) { Settings->SetPlayerStart ( className ); }

        const std::string & GetDefaultPawnClass () const { return Settings->DefaultPawnClassName; }
        const std::string & GetDefaultPlayerControllerClass () const { return Settings->DefaultPlayerControllerClassName; }
        const std::string & GetDefaultHUDClass () const { return Settings->DefaultHUDClassName; }
        const std::string & GetDefaultPlayerStartClass () const { return Settings->DefaultPlayerStartClassName; }
        FGameModeSettings * GetGameModeSettings () const { return Settings; }

        // ========== PLAYER SPAWNING ==========
        virtual CPlayerController * SpawnPlayerController ();
        virtual CPawn * SpawnDefaultPawnForController ( CPlayerController * Controller );
        virtual void RestartPlayer ( CPlayerController * Controller );
        virtual void RestartPlayerAtTransform ( CPlayerController * Controller, const FTransform & SpawnTransform );
        virtual bool ShouldSpawnPlayerAutomatically () const { return true; }

        // ========== PLAYER MANAGEMENT ==========
        const std::vector<CPlayerController *> & GetPlayerControllers () const { return PlayerControllers; }
        int32 GetNumPlayers () const { return static_cast< int32 >( PlayerControllers.size () ); }
        void RemovePlayerController ( CPlayerController * Controller );

        // ========== GAME STATE ==========
        bool IsGameStarted () const { return bGameStarted; }
        bool IsGameOver () const { return bGameOver; }
        virtual void GameOver ();
        float GetGameTime () const { return GameTime; }

        // ========== WORLD ACCESS ==========
        CWorld * GetWorld () const { return OwningWorld; }
        void SetWorld ( CWorld * World ) { OwningWorld = World; }


    protected:
        FGameModeSettings * Settings {};

        // Состояние игры
        bool bGameStarted = false;
        bool bGameOver = false;
        float GameTime = 0.0f;
        bool IsValidPawnClass ( const std::string & ClassName ) const;
        void SpawnPlayerForNewGame ();
        // Список игроков
        std::vector<CPlayerController *> PlayerControllers;

        // Владелец
        CWorld * OwningWorld = nullptr;

        // Вспомогательные методы
        virtual CActor * FindPlayerStart ( CPlayerController * Player = nullptr );
        virtual bool IsValidPlayerStart ( CActor * PlayerStart );
    };

REGISTER_CLASS_FACTORY ( CGameMode );
