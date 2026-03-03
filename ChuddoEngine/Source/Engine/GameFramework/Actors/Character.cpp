#include "Actors/Character.h"
#include "Components/Meshes/StaticMeshComponent.h"
#include "Components/Meshes/TerrainMeshComponent.h"
#include "Components/Collisions/CapsuleComponent.h"
#include "Components/Collisions/BoxComponent.h"
#include "Components/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/GravityComponent.h"
#include "World/World.h"
#include "World/Level.h"

CCharacter::CCharacter ( CObject * inOwner, const std::string & DisplayName )
    : Super ( inOwner, DisplayName )
    {
    CreateCharacterMovementComponent ();

    Capsule = AddDefaultSubObject<CCapsuleComponent> ( "Capsule" );
    Capsule->SetHalfHeight ( 9.f );
    Capsule->SetRadius ( 1.8f );
    SetRootComponent ( Capsule );

    SetDrawCollisions ( true );

    Mesh = AddDefaultSubObject<CStaticMeshComponent> ( "Mesh" );
    Mesh->AttachTo ( Capsule );
    Mesh->SetRelativeLocation ( 0.f, 0.f, 0.f );
    Mesh->CreateFallBackCube ();
    Mesh->SetScale ( 0.5f );

    Camera = AddDefaultSubObject<CCameraComponent> ( "Camera" );
    Camera->AttachTo ( Capsule );

    float PitchDownRadians = CEMath::DegreesToRadians ( -10.0f );
    FQuat DownRotation ( FVector::Right (), PitchDownRadians );
    FQuat FinalRotation = DownRotation * GetActorRotationQuat ();
    FinalRotation.Normalize ();

    Camera->SetRelativeRotation ( FinalRotation );
    Camera->SetRelativeLocation ( 0.f, 18.f, -20.f );
    
    }

void CCharacter::CreateCharacterMovementComponent ()
    {
    if (MovementComponent)
        {
        RemoveOwnedObject ( MovementComponent->GetName () );
        MovementComponent = nullptr;
        }

    MovementComponent = AddDefaultSubObject<CCharacterMovementComponent> ( "CharMov" );

    if (MovementComponent)
        {
        MovementComponent->SetOwnerPawn ( this );
        LOG_DEBUG ( "[CHARACTER] Movement component created: ", MovementComponent->GetName () );
        }
    else
        {
        LOG_ERROR ( "[CHARACTER] Failed to create movement component" );
        }
    }

void CCharacter::BeginPlay ()
    {
    Super::BeginPlay ();
    for (auto& actor : GetWorld ()->GetCurrentLevel ()->GetActors ())
        {
        for (auto & comp : actor->GetActorComponents ())
            {
            if (CTerrainMeshComponent * terMesh = dynamic_cast< CTerrainMeshComponent * >( comp ))
                {
                terrainMesh = terMesh;
                break;
                }
            }
        }
    }



void CCharacter::Tick ( float DeltaTime )
    {
    Super::Tick ( DeltaTime );
    float dist = ( terrainMesh->GetLocation () - Camera->GetLocation () ).Length ();
    static float speedTimer = 0.f;
    speedTimer += DeltaTime;
    if (speedTimer >= 1.f)
        {
        LOG_INFO (  );
        LOG_INFO ( "Distance ",dist);
        LOG_INFO (  );
        speedTimer = 0.f;
        }

    }

void CCharacter::EndPlay ()
    {
    Super::EndPlay ();
    }

void CCharacter::SetupPlayerInputComponent ( CInputComponent * InputComponent )
    {
    Super::SetupPlayerInputComponent ( InputComponent );

    if (auto * Input = GetInputComponent ())
        {
        Input->BindAxis ( "MoveForward", GLFW_KEY_W, GLFW_KEY_S,
                          [ this ] ( float value ) { MoveForward ( value ); } );
        Input->BindAxis ( "MoveRight", GLFW_KEY_D, GLFW_KEY_A,
                          [ this ] ( float value ) { MoveRight ( value ); } );
        Input->BindAxis ( "MoveUp", GLFW_KEY_E, GLFW_KEY_Q,
                          [ this ] ( float value ) { MoveUp ( value ); } );
        Input->BindAction ( "Jump", GLFW_KEY_SPACE, EInputEvent::IE_Pressed,
                            [ this ] () { Jump (); } );
        Input->BindAction ( "Spawn", GLFW_KEY_F, EInputEvent::IE_Pressed,
                            [ this ] () { SpawnCube (); } );
        }
    }

void CCharacter::MoveRight ( float value )
    {
    if (value != 0.f && MovementComponent)
        {
        FVector Direction = GetActorRotationQuat () * FVector::Right ();
        AddMovementInput ( Direction, value );
        }
    }

void CCharacter::MoveForward ( float Value )
    {
    if (Value != 0.f && MovementComponent)
        {
        FVector Direction = GetActorRotationQuat () * FVector::Forward ();
        AddMovementInput ( Direction, Value );
        }
    }

void CCharacter::MoveUp ( float Value )
    {
    if (Value != 0.f && MovementComponent)
        {
        AddMovementInput ( FVector::Up (), Value );
        }
    }

void CCharacter::Jump ()
    {
    if (auto * CharMov = dynamic_cast< CCharacterMovementComponent * >( MovementComponent ))
        {
        CharMov->Jump ();
        }
    }

bool CCharacter::IsJumping () const
    {
    if (auto * CharMov = dynamic_cast< CCharacterMovementComponent * >( MovementComponent ))
        {
        return CharMov->IsJumping ();
        }
    return false;
    }

void CCharacter::SpawnCube ()
    {
    if (!GetWorld () || !GetWorld ()->GetCurrentLevel ()) return;

    auto level = GetWorld ()->GetCurrentLevel ();
    FVector SpawnOffset = GetActorForwardVector () * 600.f;
    FVector spawnLocation = GetActorLocation () + SpawnOffset ;

    auto cubeActor = SpawnActor<CActor> ( "TestCube", spawnLocation );
    if (!cubeActor) return;

    auto cubemesh = cubeActor->AddDefaultSubObject<CStaticMeshComponent> ( "Testmesh" );
    cubeActor->BeginPlay ();
    auto box = cubeActor->AddDefaultSubObject<CBoxComponent> ( "CubeBox" );

    box->SetHalfExtents ( FVector ( 5.f, 5.f, 5.f ) );
    cubeActor->SetRootComponent ( cubemesh );
    cubemesh->SetCollisionComponent ( box );
    box->AttachTo ( cubemesh );
    box->SetChannelAsInteractable ();
    cubeActor->SetCollisionEnabled (true); // по умолчанию true
    cubeActor->SetActorLocation ( spawnLocation, true );
    cubeActor->SetMovableState ( EMovableState::DYNAMIC );
    cubemesh->UpdateTransform ();

    LOG_DEBUG ( "[CHARACTER] Test cube spawned at: ", cubeActor->GetActorLocation () );
    }