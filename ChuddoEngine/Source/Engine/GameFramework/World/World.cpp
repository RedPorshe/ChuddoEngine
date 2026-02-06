// Engine includes
#include "World/World.h"
#include "GameFramework/GameInstance.h"
#include "GameFramework/World/Level.h"

//system includes
#include <iostream>
#include <algorithm>

CWorld::CWorld ( CObject * inOwner, const std::string & displayName )
	: Super ( inOwner, displayName )
	{
		// Получаем GameInstance из владельца
	OwningGameInstance = dynamic_cast< CGameInstance * >( inOwner );

	if (OwningGameInstance)
		{
		LOG_INFO ("[WORLD] World created : ",  displayName
			, " [Owner: " , OwningGameInstance->GetName () , "]");
		}
	else if (inOwner)
		{
		LOG_INFO ( "[WORLD] World created: " , displayName
			, " [Owner: " , inOwner->GetObjectClassName () , "]");
		}
	else
		{
		LOG_INFO ( "[WORLD] World created: ",  displayName , " [No owner]");
		}
	}

CWorld::~CWorld ()
	{
	LOG_INFO ( "[WORLD] World destroyed: " , GetName ());
	DumpState ();
	// Завершаем игру если запущена
	if (bIsPlaying)
		{
		EndPlay ();
		}

		// Очищаем уровни
	Levels.clear ();
	CurrentLevel = nullptr;
	OwningGameInstance = nullptr;
	}

	// ========== LEVEL MANAGEMENT ==========

CWorld * CWorld::GetWorld ()
	{
	return CGameInstance::Get ().GetWorld ();
	}

CLevel * CWorld::CreateDefaultEmptyLevel ()
	{
	return CreateLevel<CLevel> ( "Empty Level" );
	}


void CWorld::AddLevel ( CLevel * level )
	{
	if (!level)
		return;

	// Устанавливаем этот мир как владельца уровня
	level->OwningWorld = this;


	Levels.push_back ( level );

	LOG_DEBUG( "[WORLD] Level added: " , level->GetName ()
		, " (Total levels: " , Levels.size (), ")");
	}

bool CWorld::RemoveLevel ( const std::string & levelName )
	{
		// Ищем уровень по имени
	auto level = this->FindObjectByName ( levelName );
	if (!level)
		{
		LOG_WARN( "[WORLD] Level not found: " , levelName);
		return false;
		}

		// Приводим к CLevel*
	CLevel * levelPtr = dynamic_cast< CLevel * >( level );
	if (!levelPtr)
		{
		LOG_ERROR( "[WORLD] ERROR: Object '" , levelName
			, "' is not a CLevel!");
		return false;
		}

		// Ищем уровень в векторе
	auto it = std::find ( Levels.begin (), Levels.end (), levelPtr );
	if (it == Levels.end ())
		{
		LOG_ERROR( "[WORLD] ERROR: Level '" , levelName
			, "' not found in Levels vector!");
		return false;
		}

		// Обработка текущего уровня
	if (CurrentLevel == levelPtr)
		{
		LOG_DEBUG ( "[WORLD] Removing current level: ", levelName );

		// Если есть другие уровни, выбираем новый текущий
		if (Levels.size () > 1)
			{
				// Определяем индекс удаляемого уровня
			auto levelIndex = std::distance ( Levels.begin (), it );

			// Выбираем следующий уровень, или предыдущий если удаляем последний
			if (levelIndex < static_cast< int > ( Levels.size () ) - 1)
				{
					// Выбираем следующий уровень
				SetCurrentLevel ( Levels[ levelIndex + 1 ] );
				}
			else
				{
					// Удаляем последний уровень, выбираем предыдущий
				SetCurrentLevel ( Levels[ levelIndex - 1 ] );
				}
			}
		else
			{
				// Это единственный уровень
			SetCurrentLevel ( nullptr );
			}
		}

		// Завершаем уровень если мир играет
	if (bIsPlaying)
		{
		levelPtr->EndPlay ();
		}

		// Удаляем из вектора
	Levels.erase ( it );

	// Устанавливаем указатель на мир в nullptr перед удалением
	levelPtr->OwningWorld = nullptr;

	// Удаляем из OwnedObjects
	bool removed = RemoveOwnedObject ( levelName );

	if (removed)
		{
		LOG_DEBUG( "[WORLD] Level removed: " , levelName
			, " (Remaining levels: " , Levels.size (), ")");
		}

	return removed;
	}

bool CWorld::RemoveLevel ( CLevel * level )
	{
	if (!level)
		return false;

	// Проверяем, что уровень принадлежит этому миру
	if (level->OwningWorld != this)
		{
		LOG_ERROR( "[WORLD] ERROR: Level '" , level->GetName ()
			, "' does not belong to this world!");
		return false;
		}

	return RemoveLevel ( level->GetName () );
	}

void CWorld::SetCurrentLevel ( CLevel * level )
	{
		// Если устанавливаем nullptr
	if (!level)
		{
		if (CurrentLevel)
			{
			LOG_DEBUG( "[WORLD] Current level cleared. Was: "
				, CurrentLevel->GetName ());
			}
		CurrentLevel = nullptr;
		return;
		}

		// Проверяем, что уровень принадлежит этому миру
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
		LOG_ERROR( "[WORLD] ERROR: Level '" , level->GetName ()
			, "' does not belong to this world!");
		return;
		}

		// Если уровень уже текущий
	if (CurrentLevel == level)
		{
		LOG_WARN( "[WORLD] Level '" , level->GetName ()
			, "' is already current");
		return;
		}

		// Завершаем предыдущий уровень если мир играет
	if (bIsPlaying && CurrentLevel)
		{
		CurrentLevel->EndPlay ();
		}

		// Устанавливаем новый уровень
	CurrentLevel = level;

	// Запускаем новый уровень если мир играет
	if (bIsPlaying)
		{
		level->BeginPlay ();
		}

	LOG_DEBUG( "[WORLD] Current level set to: ",  level->GetName ());
	}

	// ========== WORLD LIFECYCLE ==========

void CWorld::BeginPlay ()
	{
	if (bIsPlaying)
		{
		LOG_WARN( "[WORLD] ERROR: World is already playing!");
		return;
		}

	bIsPlaying = true;
	LOG_DEBUG( "[WORLD] BeginPlay: " , GetName ());

	// Запускаем все уровни
	for (auto & level : Levels)
		{
		level->BeginPlay ();
		}
	}

void CWorld::Tick ( float deltaTime )
	{
	if (!bIsPlaying)
		{
		LOG_DEBUG( "[WORLD] World is not playing, skipping tick");
		return;
		}

	LOG_DEBUG( "[WORLD] Tick: ",  GetName () , " (delta: " , deltaTime , ")");

	// Обновляем текущий уровень
	if (CurrentLevel)
		{
		CurrentLevel->Tick ( deltaTime );
		}
	else
		{
		LOG_WARN ("[WORLD] No current level to tick");
		}
	}

void CWorld::EndPlay ()
	{
	if (!bIsPlaying)
		return;

	bIsPlaying = false;
	LOG_DEBUG( "[WORLD] EndPlay: " , GetName ());

	// Завершаем все уровни
	for (auto & level : Levels)
		{
		level->EndPlay ();
		}
	}

	// ========== SEARCH/QUERY ==========

CObject * CWorld::FindObjectByName ( const std::string & name ) const
	{
		// Ищем в уровнях
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
		// Ищем в уровнях
	for (const auto & level : Levels)
		{
		CObject * found = level->FindObjectByUUID ( uuid );
		if (found)
			return found;
		}

	return nullptr;
	}

template<typename T>
T * CWorld::FindObjectOfType () const
	{
	for (const auto & level : Levels)
		{
			// Реализуем когда будет система компонентов
			// T* found = level->FindObjectOfType<T>();
			// if (found) return found;
		}

	return nullptr;
	}

	// ========== DEBUG/UTILS ==========

void CWorld::DumpState () const
	{
	LOG_DEBUG( "=== WORLD STATE ===");
	LOG_DEBUG( "Name: " , GetName ()) ;
	LOG_DEBUG( "UUID: " , GetShortUUID ()) ;
	LOG_DEBUG( "GameInstance: "
		, ( OwningGameInstance ? OwningGameInstance->GetName () : "None" ) );
	LOG_DEBUG( "Is Playing: " , ( bIsPlaying ? "Yes" : "No" ));
	LOG_DEBUG( "Current Level: "
			   , ( CurrentLevel ? CurrentLevel->GetName () : "None" ) );
	LOG_DEBUG ( "Total Levels: " , Levels.size ());

	for (size_t i = 0; i < Levels.size (); ++i)
		{
		LOG_DEBUG( "  [" , i , "] " , Levels[ i ]->GetName ()
			, " (Active: " , ( Levels[ i ] == CurrentLevel ? "Yes" : "No" ) );
		}

	LOG_DEBUG( "===================");
	}

