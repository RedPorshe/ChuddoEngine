// World.h
#pragma once
#include "Core/Object.h"
#include <vector>
#include <memory>

// Forward declarations
class CGameInstance;
class CLevel;

class CWorld : public CObject
    {
    CHUDDO_DECLARE_CLASS ( CWorld, CObject )

    private:
        CGameInstance * OwningGameInstance = nullptr;
        std::vector<std::unique_ptr<CLevel>> Levels;
        CLevel * CurrentLevel = nullptr;

    public:
        // Конструктор должен принимать CObject* (для фабрики)
        CWorld ( CObject * inOwner = nullptr, const std::string & displayName = "World" );
        virtual ~CWorld ();

        // ========== GAME INSTANCE ACCESS ==========
        CGameInstance * GetGameInstance () const { return OwningGameInstance; }

        CWorld * GetWorld ();

        // ========== LEVEL MANAGEMENT ==========
        CLevel * CreateLevel ( const std::string & levelName = "Level" );
        void AddLevel ( std::unique_ptr<CLevel> level );
        bool RemoveLevel ( const std::string & levelName );
        bool RemoveLevel ( CLevel * level );

        void SetCurrentLevel ( CLevel * level );
        CLevel * GetCurrentLevel () const { return CurrentLevel; }

        size_t GetNumLevels () const { return Levels.size (); }
        bool HasLevels () const { return !Levels.empty (); }

        // ========== WORLD LIFECYCLE ==========
        virtual void BeginPlay ();
        virtual void Tick ( float deltaTime );
        virtual void EndPlay ();

        // ========== SEARCH/QUERY ==========
        CObject * FindObjectByName ( const std::string & name ) const;
        CObject * FindObjectByUUID ( const std::string & uuid ) const;

        template<typename T>
        T * FindObjectOfType () const;

        // ========== DEBUG/UTILS ==========
        virtual void DumpState () const;

    protected:
        bool bIsPlaying = false;
    };

REGISTER_CLASS_FACTORY ( CWorld );
