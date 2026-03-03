#include "World/World.h"
#include "GameFramework/GameInstance.h"
#include "GameFramework/World/Level.h"
#include "GameFramework/GameMode.h"
#include "Render/RenderInfo.h"
#include "Render/Window.h"
#include "Actors/Actor.h"
#include "Camera/CameraComponent.h"
#include "Core/Engine.h"
#include <algorithm>

CWorld::CWorld ( CObject * inOwner, const std::string & displayName )
    : Super ( inOwner, displayName )
    {
    OwningGameInstance = dynamic_cast< CGameInstance * >( inOwner );
    m_RenderInfo =  FRenderInfo ();
    m_CameraInfo =  FCameraInfo ();
       // 1. Создаем GameMode если его нет
    if (!CurrentGameMode)
        {
        LOG_WARN ( "[WORLD] No GameMode found, creating default GameMode" );
        CreateGameMode<CGameMode> ( "GameModeBase" );
        }
    }

CWorld::~CWorld ()
    {
    DumpState ();

    if (bIsPlaying)
        {
        EndPlay ();
        }

    CurrentGameMode = nullptr;
    Levels.clear ();
    CurrentLevel = nullptr;
    OwningGameInstance = nullptr;
  
    }

    // ========== LEVEL MANAGEMENT ==========

void CWorld::AddLevel ( CLevel * level )
    {
    if (!level) return;

    level->OwningWorld = this;
    Levels.push_back ( level );

    LOG_DEBUG ( "[WORLD] Level added: ", level->GetName (),
                " (Total levels: ", Levels.size (), ")" );
    }

bool CWorld::RemoveLevel ( const std::string & levelName )
    {
    auto level = FindObjectByName ( levelName );
    if (!level)
        {
        LOG_WARN ( "[WORLD] Level not found: ", levelName );
        return false;
        }

    CLevel * levelPtr = dynamic_cast< CLevel * >( level );
    if (!levelPtr)
        {
        LOG_ERROR ( "[WORLD] ERROR: Object '", levelName, "' is not a CLevel!" );
        return false;
        }

    auto it = std::find ( Levels.begin (), Levels.end (), levelPtr );
    if (it == Levels.end ())
        {
        LOG_ERROR ( "[WORLD] ERROR: Level '", levelName, "' not found in Levels vector!" );
        return false;
        }

    if (CurrentLevel == levelPtr)
        {
        LOG_DEBUG ( "[WORLD] Removing current level: ", levelName );

        if (Levels.size () > 1)
            {
            auto levelIndex = std::distance ( Levels.begin (), it );
            if (levelIndex < static_cast< int > ( Levels.size () ) - 1)
                {
                SetCurrentLevel ( Levels[ levelIndex + 1 ] );
                }
            else
                {
                SetCurrentLevel ( Levels[ levelIndex - 1 ] );
                }
            }
        else
            {
            SetCurrentLevel ( nullptr );
            }
        }

    if (bIsPlaying)
        {
        levelPtr->EndPlay ();
        }

    Levels.erase ( it );
    levelPtr->OwningWorld = nullptr;

    bool removed = RemoveOwnedObject ( levelName );
    if (removed)
        {
        LOG_DEBUG ( "[WORLD] Level removed: ", levelName,
                    " (Remaining levels: ", Levels.size (), ")" );
        }

    return removed;
    }

bool CWorld::RemoveLevel ( CLevel * level )
    {
    if (!level) return false;

    if (level->OwningWorld != this)
        {
        LOG_ERROR ( "[WORLD] ERROR: Level '", level->GetName (),
                    "' does not belong to this world!" );
        return false;
        }

    return RemoveLevel ( level->GetName () );
    }

void CWorld::SetCurrentLevel ( CLevel * level )
    {
    if (!level)
        {
        if (CurrentLevel)
            {
            LOG_DEBUG ( "[WORLD] Current level cleared. Was: ", CurrentLevel->GetName () );
            }
        CurrentLevel = nullptr;
        return;
        }

    bool belongsToWorld = false;
    for (auto lvl : Levels)
        {
        if (lvl == level)
            {
            belongsToWorld = true;
            break;
            }
        }

    if (!belongsToWorld)
        {
        LOG_ERROR ( "[WORLD] ERROR: Level '", level->GetName (),
                    "' does not belong to this world!" );
        return;
        }

    if (CurrentLevel == level)
        {
        LOG_WARN ( "[WORLD] Level '", level->GetName (), "' is already current" );
        return;
        }

    if (bIsPlaying && CurrentLevel)
        {
        CurrentLevel->EndPlay ();
        }

    CurrentLevel = level;

    if (bIsPlaying)
        {
        level->BeginPlay ();
        }

    LOG_DEBUG ( "[WORLD] Current level set to: ", level->GetName () );
    }

    // ========== GAME MODE MANAGEMENT ==========

void CWorld::SetGameMode ( CGameMode * NewGameMode )
    {
    if (CurrentGameMode == NewGameMode) return;

    if (CurrentGameMode)
        {
        if (bIsPlaying)
            {
            CurrentGameMode->EndGame ();
            }
        RemoveOwnedObject ( CurrentGameMode->GetName () );
        }

    CurrentGameMode = NewGameMode;

    if (CurrentGameMode)
        {
        CurrentGameMode->SetWorld ( this );
        if (bIsPlaying)
            {
            CurrentGameMode->StartPlay ();
            }
        LOG_DEBUG ( "[WORLD] GameMode set to: ", CurrentGameMode->GetName () );
        }
    }

    // ========== WORLD LIFECYCLE ==========

void CWorld::BeginPlay ()
    {
    if (bIsPlaying)
        {
        LOG_WARN ( "[WORLD] World is already playing!" );
        return;
        }

    bIsPlaying = true;
    LOG_DEBUG ( "[WORLD] BeginPlay: ", GetName () );
    m_RenderInfo = CollectRenderInfo ();
 

    if (CurrentGameMode)
        {
        CurrentGameMode->StartPlay ();
        }

    for (auto & level : Levels)
        {
        level->BeginPlay ();
        }
    }

void CWorld::Tick ( float deltaTime )
    {
    CurrentDeltaTime = deltaTime;
    m_RenderInfo = CollectRenderInfo ();
    if (!bIsPlaying) return;

    // Tick GameMode
    if (CurrentGameMode)
        {
        CurrentGameMode->Tick ( deltaTime );
        }

        // Tick текущий уровень
    if (CurrentLevel)
        {
        CurrentLevel->Tick ( deltaTime );
        }
    }

void CWorld::EndPlay ()
    {
    if (!bIsPlaying) return;

    bIsPlaying = false;
    LOG_DEBUG ( "[WORLD] EndPlay: ", GetName () );

    // Завершаем GameMode
    if (CurrentGameMode)
        {
        CurrentGameMode->EndGame ();
        }

        // Завершаем уровни
    for (auto & level : Levels)
        {
        level->EndPlay ();
        }
    }

    // ========== RENDER INFO ==========

FRenderInfo CWorld::CollectRenderInfo ()
    {
    m_RenderInfo.Clear ();
    m_CameraInfo.Clear ();
    // 1. Собираем камеру
    m_CameraInfo = FindActiveCamera ();
   
    m_RenderInfo.Camera = m_CameraInfo;
    // 2. Собираем меши, террейны и коллизии для рендера
    if (CurrentLevel)
        {
        const auto & actors = CurrentLevel->GetActors ();

        for (CActor * actor : actors)
            {
            if (!actor || actor->IsHiddenInGame ()) continue;

            FRenderCollection renderCollection = actor->GetRenderInfo ();

            // Добавляем меши
            for (const auto & mesh : renderCollection.Meshes)
                {
                if (mesh.IsValid ())
                    {
                    m_RenderInfo.AddMesh ( mesh );
                    }
                }

                // Добавляем террейны
            for (const auto & terrain : renderCollection.Terrains)
                {
                if (terrain.IsValid ())
                    {
                    m_RenderInfo.AddTerrain ( terrain );
                    }
                }

                // Добавляем отладочные коллизии
            for (const auto & collision : renderCollection.DebugCollisions)
                {
                if (collision.IsValid ())
                    {
                    m_RenderInfo.AddDebugCollision ( collision );
                    }
                }

            for (const auto & TerWire : renderCollection.TerrainWireframes)
                {
                m_RenderInfo.AddTerrainWireframe ( TerWire );
                }
            }
        }

        // 3. Устанавливаем флаги
    m_RenderInfo.HasInfo = !m_RenderInfo.RenderMeshes.empty () || !m_RenderInfo.Terrains.empty ();
    m_RenderInfo.bDrawCollisions = HasAnyActorWithDebugCollisions ();

    return m_RenderInfo;
    }

bool CWorld::HasAnyActorWithDebugCollisions () const
    {
    if (!CurrentLevel) return false;

    for (CActor * actor : CurrentLevel->GetActors ())
        {
        if (actor && actor->IsDrawCollisionsEnabled ())
            return true;
        }
    return false;
    }

FCameraInfo CWorld::FindActiveCamera ()
    {
    m_CameraInfo.Clear ();

    if (CurrentLevel)
        {
        const auto & actors = CurrentLevel->GetActors ();
        for (CActor * actor : actors)
            {
            CCameraComponent * camera = actor->FindComponent<CCameraComponent> ();
            if (camera && camera->IsVisible ())
                {
                float aspectRatio = CEngine::Get ().GetWindow ()->GetAspectRatio ();
                if (aspectRatio <= 0.0f) aspectRatio = 16.0f / 9.0f;
                m_CameraInfo = camera->GetCameraInfo ( aspectRatio );
                return m_CameraInfo;
                }
            }
        }

        // Тестовая камера
    float aspectRatio = CEngine::Get ().GetWindow ()->GetAspectRatio ();
    if (aspectRatio <= 0.0001f) aspectRatio = 16.0f / 9.0f;

    m_CameraInfo.Location = { 10.f, 5.f, 10.f };
    m_CameraInfo.ViewTarget = { 0.f, 0.f, 0.f };
    m_CameraInfo.NearPlane = 0.1f;
    m_CameraInfo.FarPlane = 1000.f;
    m_CameraInfo.FOV = 90.f;
    m_CameraInfo.ViewMatrix = FMat4::LookAtMatrix ( m_CameraInfo.Location, m_CameraInfo.ViewTarget, -FVector::Up () );
    m_CameraInfo.ProjectionMatrix = FMat4::PerspectiveMatrix (
        m_CameraInfo.FOV * CEMath::DEG_TO_RAD,
        aspectRatio,
        m_CameraInfo.NearPlane,
        m_CameraInfo.FarPlane
    );

    return  m_CameraInfo;
    }

    // ========== SEARCH/QUERY ==========

CObject * CWorld::FindObjectByName ( const std::string & name ) const
    {
    for (const auto & level : Levels)
        {
        CObject * found = level->FindObjectByName ( name );
        if (found)
            return found;
        }

    return nullptr;
    }

CObject * CWorld::FindObjectByUUID ( const std::string & uuid ) const
    {
    for (const auto & level : Levels)
        {
        CObject * found = level->FindObjectByUUID ( uuid );
        if (found)
            return found;
        }

    return nullptr;
    }

    // ========== DEBUG/UTILS ==========

void CWorld::DumpState () const
    {
    LOG_DEBUG ( "=== WORLD STATE ===" );
    LOG_DEBUG ( "Name: ", GetName () );
    LOG_DEBUG ( "UUID: ", GetShortUUID () );
    LOG_DEBUG ( "GameInstance: ", ( OwningGameInstance ? OwningGameInstance->GetName () : "None" ) );
    LOG_DEBUG ( "Is Playing: ", ( bIsPlaying ? "Yes" : "No" ) );
    LOG_DEBUG ( "Current Level: ", ( CurrentLevel ? CurrentLevel->GetName () : "None" ) );
    LOG_DEBUG ( "Has GameMode: ", ( CurrentGameMode ? "Yes" : "No" ) );

    if (CurrentGameMode)
        {
        LOG_DEBUG ( "GameMode: ", CurrentGameMode->GetName () );
        LOG_DEBUG ( "GameMode Class: ", CurrentGameMode->GetObjectClassName () );
        }

    LOG_DEBUG ( "Total Levels: ", Levels.size () );

    for (size_t i = 0; i < Levels.size (); ++i)
        {
        LOG_DEBUG ( "  [", i, "] ", Levels[ i ]->GetName (),
                    " (Active: ", ( Levels[ i ] == CurrentLevel ? "Yes" : "No" ), ")" );
        }

    LOG_DEBUG ( "===================" );
    }