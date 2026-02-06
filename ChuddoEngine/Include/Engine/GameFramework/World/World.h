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
        std::vector<CLevel*> Levels;
        CLevel * CurrentLevel = nullptr;

    public:
        // Конструктор должен принимать CObject* (для фабрики)
        CWorld ( CObject * inOwner = nullptr, const std::string & displayName = "World" );
        virtual ~CWorld ();

        // ========== GAME INSTANCE ACCESS ==========
        CGameInstance * GetGameInstance () const { return OwningGameInstance; }

        CWorld * GetWorld ();

        CLevel * CreateDefaultEmptyLevel ();

        // ========== LEVEL MANAGEMENT ==========
       // CLevel * CreateLevel ( const std::string & levelName = "Level" );
        void AddLevel ( CLevel* level );
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



        template<typename LevelType ,typename... Args>
        LevelType* CreateLevel ( const std::string & name = "Actor", Args&&... args );
           

    protected:
        bool bIsPlaying = false;
    };

REGISTER_CLASS_FACTORY ( CWorld );

template<typename LevelType, typename... Args>
LevelType* CWorld::CreateLevel ( const std::string & name  , Args&&... args )
    {
    static_assert( std::is_base_of<CLevel, LevelType>::value,
                   "Level type must be derived from CLevel" );

    LevelType * newLevel = this->AddSubObject<LevelType> ( name, std::forward<Args> ( args )... );
    if (!newLevel)
        {
        LOG_ERROR ( "[WORLD] Error: Failed to spawn actor '", name, "'" );
        
        return nullptr;
        }
    Levels.push_back ( newLevel );
    if(CurrentLevel==nullptr)
        { SetCurrentLevel ( newLevel ); }
    return newLevel;
    }