#include "Core/World.h"
#include "Core/Level.h"

CWorld::CWorld ( const CObject * Owner, const std::string & inName ) :CObject ( Owner, inName )
	{
	
	}



CWorld::~CWorld ()
	{

	}

void CWorld::Tick ( float DeltaTime )
	{

	if (PersistentLevel)
		{
		if (CurrentLevel == PersistentLevel)
			{
			std::cout << "tick in current level '"<< CurrentLevel->GetName()<<"'\n";
			CurrentLevel->Tick ( DeltaTime );
			}
		else
			{
			std::cout << "tick in persistent level '" <<PersistentLevel->GetName()<< "'\n";
			PersistentLevel->Tick ( DeltaTime );
			}
		}
	}

void CWorld::BeginPlay ()
	{
	if (PersistentLevel)
		{
		if (CurrentLevel == PersistentLevel)
			{
			std::cout << "BeginPlay in Current Level  '" << CurrentLevel->GetName () << "'\n";
			CurrentLevel->BeginPlay ();
			}
		else
			{
			std::cout << "BeginPlay in Persistent Level '" << PersistentLevel->GetName () << "'\n";
			PersistentLevel->BeginPlay ();
			}
		}
	}

void CWorld::LoadLevel ( const std::string & LevelName )
	{
		// For simplicity, we will just create a new level instance here.
	for (auto level : LoadedLevels)
		{
		if (level->GetName () == LevelName)
			{
			std::cout << "Level '" << LevelName << "' is already loaded." << std::endl;
			SetCurrentLevel ( level );
			return;
			}
		}
		// Create level with no owner (nullptr), then AddOwnedObject will set proper OwnerObject
	auto NewLevelUP = std::make_unique<CLevel> ( nullptr, LevelName );
	CLevel * NewLevel = static_cast< CLevel * >( AddOwnedObject ( std::move ( NewLevelUP ) ) );
	LoadedLevels.push_back ( NewLevel );
	if (PersistentLevel != nullptr)
		{
		SetCurrentLevel ( NewLevel );
		}
	else
		{
		SetPersistentLevel ( NewLevel );
		}
	}

void CWorld::SetCurrentLevel ( CLevel * NewLevel )
	{
	if (CurrentLevel != NewLevel)
		{
		UnloadLevel ( CurrentLevel ? CurrentLevel->GetName () : "" );
		CurrentLevel = NewLevel;
		std::cout << "Current Level set to '" << CurrentLevel->GetName () << "' in World '" << GetName () << "'." << std::endl;
		}
	}

void CWorld::UnloadLevel ( const std::string & LevelName )
	{
	auto it = std::find_if ( LoadedLevels.begin (), LoadedLevels.end (),
							 [ &LevelName ] ( CLevel * Level ) { return Level->GetName () == LevelName; } );
	if (it != LoadedLevels.end ())
		{
		CLevel * LevelToUnload = *it;
		LoadedLevels.erase ( it );
		if (LevelToUnload == CurrentLevel)
			{
			CurrentLevel = nullptr;
			}
		if (LevelToUnload == PersistentLevel)
			{
			PersistentLevel = nullptr;
			}
			// Request removal via AddOwnedObject logic - do NOT delete directly
		RemoveOwnedObject ( LevelToUnload, true );
		std::cout << "Unloaded Level '" << LevelName << "' from World '" << GetName () << "'." << std::endl;
		}
	else
		{
		std::cout << "Level '" << LevelName << "' not found in Loaded Levels." << std::endl;
		}
	}
