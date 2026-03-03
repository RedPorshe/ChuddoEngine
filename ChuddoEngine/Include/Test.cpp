#include "Test.h"
#include "Actors/Pawn.h"
#include "Actors/PlayerController.h"
#include "World/World.h"

CTestLevel::CTestLevel ( CObject * inOwner, const std::string & inDisplayName ) :Super ( inOwner, inDisplayName )
    {
    
    }

CTestLevel::~CTestLevel ()
    {}

void CTestLevel::BeginPlay ()
    {
    Super::BeginPlay ();

    // Теперь World и GameMode точно существуют
    if (GetOwningWorld () && GetOwningWorld ()->GetGameMode ())
        {
        GetOwningWorld ()->GetGameMode ()->SetDefaultPlayerControllerClass ( "CPlayerController" );
        }

    TestCharacter = SpawnActor<CPawn> ( "TestCharacter1" );
    if (TestCharacter)
        {
        TestCharacter->SetActorLocation ( FVector ( 0.0f, 0.0f, 100.0f ) );

        CController * PlayerController = SpawnActor<CController> ( "TestPlayerController" );
        if (PlayerController)
            {
            PlayerController->Possess ( TestCharacter );
            }
        }
    }

void CTestLevel::EndPlay ()
    {
    Super::EndPlay ();
    }

void CTestLevel::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    }
