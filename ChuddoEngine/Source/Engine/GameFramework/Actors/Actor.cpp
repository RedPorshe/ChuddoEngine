#include "Actors/Actor.h"
#include "World/Level.h"
#include "World/World.h"
#include "GameInstance.h"
#include "GameFramework/Components/BaseComponent.h"
#include "Components/SceneComponent.h"

CActor::CActor ( CObject * owner, const std::string & inName ) : CObject ( owner, inName )
    {
    RootComponent = AddDefaultSubObject<CTransformComponent> ( inName + "_Transform" );
    }

CActor::~CActor ()
    {
    ActorComponents.clear ();
    }

void CActor::BeginPlay ()
    {
    LOG_DEBUG ( "[ACTOR] BeginPlay: ", GetName () );
    for (auto comp : ActorComponents)
        {
        comp->OnBeginPlay ();
        }
    }

void CActor::Tick ( float deltaTime )
    {
        // Обработка интерполяции позиции
    if (bIsLerpingLocation && RootComponent)
        {
        LocationLerpAlpha += deltaTime * LerpSpeed;

        if (LocationLerpAlpha >= 1.0f)
            {
                // Интерполяция завершена
            LocationLerpAlpha = 1.0f;
            bIsLerpingLocation = false;
            RootComponent->SetLocation ( TargetLocation );
            }
        else
            {
                // Линейная интерполяция
            FVector currentLocation = LerpStartLocation + ( TargetLocation - LerpStartLocation ) * LocationLerpAlpha;
            RootComponent->SetLocation ( currentLocation );
            }
        }

        // Обработка интерполяции вращения (Slerp - сферическая интерполяция)
    if (bIsLerpingRotation && RootComponent)
        {
        RotationLerpAlpha += deltaTime * LerpSpeed;

        if (RotationLerpAlpha >= 1.0f)
            {
                // Интерполяция завершена
            RotationLerpAlpha = 1.0f;
            bIsLerpingRotation = false;
            RootComponent->SetRotation ( TargetRotation );
            }
        else
            {
                // Сферическая линейная интерполяция (Slerp)
            FQuat currentRotation = FQuat::Slerp ( LerpStartRotation, TargetRotation, RotationLerpAlpha );
            currentRotation.Normalize ();
            RootComponent->SetRotation ( currentRotation );
            }
        }

        // Вызов Tick для всех компонентов
    for (auto comp : ActorComponents)
        {
        if (comp && comp->GetOwner () == this)
            {
            comp->Tick ( deltaTime );
            }
        }
    }

void CActor::EndPlay ()
    {
    LOG_DEBUG ( "[ACTOR] EndPlay: ", GetName () );
    }

CLevel * CActor::GetLevel () const
    {
    return dynamic_cast< CLevel * >( GetOwner () );
    }

CWorld * CActor::GetWorld () const
    {
    return CGameInstance::Get ().GetWorld ();
    }

void CActor::SetRootComponent ( CTransformComponent * NewRoot )
    {
    if (!NewRoot)
        {
        LOG_WARN ( "Cannot set nullptr as RootComponent" );
        return;
        }

    if (NewRoot->GetOwner () != this)
        {
        LOG_WARN ( "RootComponent must belong to this actor" );
        return;
        }

    if (RootComponent == NewRoot)
        {
        return;
        }

    CTransformComponent * OldRoot = RootComponent;
    RootComponent = NewRoot;

    if (OldRoot)
        {
            // Прикрепляем старый корень к новому
        NewRoot->AttachComponentToComponent ( OldRoot );
        LOG_DEBUG ( "Changed RootComponent from '", OldRoot->GetName ()
                    , "' to '", NewRoot->GetName (), "'" );
        }
    else
        {
        LOG_DEBUG ( "Set '", NewRoot->GetName ()
                    , "' as RootComponent for actor '", GetName (), "'" );
        }
    }

void CActor::Destroy ()
    {
    if (bIsPendingToDestroy)
        {
        LOG_WARN ( "Actor: ", GetName (), " already marked to destroy" );
        return;
        }
    auto level = GetLevel ();
    if (level)
        {
        level->DestroyActor ( GetName () );
        }
    }

void CActor::SetPendingToDestroy ()
    {
    if (bIsPendingToDestroy)
        {
        LOG_WARN ( "Actor: ", GetName (), " already marked to destroy" );
        return;
        }
    LOG_DEBUG ( "Actor:", GetName (), " is marked to destroy" );
    bIsPendingToDestroy = true;
    }

CBaseComponent * CActor::AddDefaultSubObject ( const std::string & className, const std::string & desiredDisplayName )
    {
    auto CompToAdd = CObject::AddSubObjectByClass ( className, desiredDisplayName );
    if (CBaseComponent * compToReturn = dynamic_cast< CBaseComponent * >( CompToAdd ))
        {
        return compToReturn;
        }
    return nullptr;
    }

FVector CActor::GetActorLocation ()
    {
    FVector result {};
    if (GetRootComponent () != nullptr) result = RootComponent->GetLocation ();
    return result;
    }

FVector CActor::GetActorRotation ()
    {
    FVector RotVec = GetActorRotationQuat ().GetEulerAngles ();
    return FVector (
        CEMath::RadiansToDegrees ( RotVec.x ),
        CEMath::RadiansToDegrees ( RotVec.y ),
        CEMath::RadiansToDegrees ( RotVec.z )
    );
    }

FVector CActor::GetActorScale ()
    {
    FVector result {};
    if (GetRootComponent () != nullptr) result = RootComponent->GetScale ();
    return result;
    }

FQuat CActor::GetActorRotationQuat ()
    {
    FQuat result {};
    if (GetRootComponent () != nullptr) result = RootComponent->GetRotationQuat ();
    return result;
    }

void CActor::SetActorLocation ( const FVector & InLocation )
    {
    if (RootComponent)
        {
        RootComponent->SetLocation ( InLocation );
        // Сбрасываем интерполяцию позиции
        bIsLerpingLocation = false;
        LocationLerpAlpha = 0.0f;
        }
    }

void CActor::SetActorLocation ( float inX, float inY, float inZ )
    {
    SetActorLocation ( FVector ( inX, inY, inZ ) );
    }

void CActor::SetActorScale ( const FVector & InScale )
    {
    if (RootComponent)
        {
        RootComponent->SetScale ( InScale );
        }
    }

void CActor::SetActorScale ( float inX, float inY, float inZ )
    {
    SetActorScale ( FVector ( inX, inY, inZ ) );
    }

void CActor::SetActorScale ( float InScale )
    {
    SetActorScale ( InScale, InScale, InScale );
    }

void CActor::SetActorRotation ( const FVector & inRotation )
    {
    FQuat rotationQuat = FQuat::FromEulerAngles (
        CEMath::DegreesToRadians ( inRotation.x ),
        CEMath::DegreesToRadians ( inRotation.y ),
        CEMath::DegreesToRadians ( inRotation.z )
    );
    SetActorRotation ( rotationQuat );
    }

void CActor::SetActorRotation ( const FQuat & inRotation )
    {
    if (RootComponent)
        {
        RootComponent->SetRotation ( inRotation );
        // Сбрасываем интерполяцию вращения
        bIsLerpingRotation = false;
        RotationLerpAlpha = 0.0f;
        }
    }

void CActor::SetActorRotation ( float inX, float inY, float inZ )
    {
    SetActorRotation ( FVector ( inX, inY, inZ ) );
    }

void CActor::MoveActor ( const FVector & Delta, bool Interpolate )
    {
    if (!RootComponent)
        return;

    if (!Interpolate)
        {
            // Мгновенное перемещение
        FVector currentLocation = RootComponent->GetLocation ();
        RootComponent->SetLocation ( currentLocation + Delta );
        return;
        }

        // Интерполированное перемещение
    FVector currentLocation = RootComponent->GetLocation ();
    TargetLocation = currentLocation + Delta;
    LerpStartLocation = currentLocation;
    LocationLerpAlpha = 0.0f;  // Используем LocationLerpAlpha
    bIsLerpingLocation = true;
    // НЕ сбрасываем bIsLerpingRotation - они могут работать одновременно
    }

void CActor::RotateActor ( const FVector & DeltaRotation, bool Interpolate )
    {
    if (!RootComponent)
        return;

    // Конвертируем дельту вращения в кватернион
    FQuat deltaQuat = FQuat::FromEulerAngles (
        CEMath::DegreesToRadians ( DeltaRotation.x ),
        CEMath::DegreesToRadians ( DeltaRotation.y ),
        CEMath::DegreesToRadians ( DeltaRotation.z )
    );

    // Получаем текущее вращение
    FQuat currentQuat = RootComponent->GetRotationQuat ();

    // Вычисляем целевое вращение
    FQuat targetQuat = currentQuat * deltaQuat;
    targetQuat.Normalize ();

    if (!Interpolate)
        {
            // Мгновенное вращение
        RootComponent->SetRotation ( targetQuat );
        return;
        }

        // Интерполированное вращение
    TargetRotation = targetQuat;
    LerpStartRotation = currentQuat;
    RotationLerpAlpha = 0.0f;  // Используем RotationLerpAlpha
    bIsLerpingRotation = true;
    // НЕ сбрасываем bIsLerpingLocation - они могут работать одновременно
    }

void CActor::AddActorWorldOffset ( const FVector & DeltaLocation, bool Interpolate )
    {
        // Мировое смещение - просто добавляем к мировой позиции
    MoveActor ( DeltaLocation, Interpolate );
    }

void CActor::AddActorLocalOffset ( const FVector & DeltaLocation, bool Interpolate )
    {
    if (!RootComponent)
        return;

    // Локальное смещение: учитываем вращение актора
    // Правильный порядок: worldDelta = rotation * DeltaLocation
    FQuat rotation = RootComponent->GetRotationQuat ();
    rotation.Normalize ();  // Убедимся, что кватернион нормализован

    // Преобразуем локальное смещение в мировое пространство
    FVector worldDelta = rotation * DeltaLocation;

    // Добавляем смещение в мировом пространстве
    AddActorWorldOffset ( worldDelta, Interpolate );
    }

void CActor::AddActorWorldRotation ( const FQuat & DeltaRotation, bool Interpolate )
    {
    if (!RootComponent)
        return;

    // Получаем текущее вращение
    FQuat currentQuat = RootComponent->GetRotationQuat ();

    // Мировое вращение: умножаем справа
    FQuat newRotation = currentQuat * DeltaRotation;
    newRotation.Normalize ();

    if (!Interpolate)
        {
        RootComponent->SetRotation ( newRotation );
        return;
        }

        // Интерполированное вращение
    TargetRotation = newRotation;
    LerpStartRotation = currentQuat;
    RotationLerpAlpha = 0.0f;
    bIsLerpingRotation = true;
    // Не сбрасываем bIsLerpingLocation
    }

void CActor::AddActorLocalRotation ( const FQuat & DeltaRotation, bool Interpolate )
    {
    if (!RootComponent)
        return;

    // Получаем текущее вращение
    FQuat currentQuat = RootComponent->GetRotationQuat ();

    // Локальное вращение: умножаем слева (порядок важен!)
    FQuat newRotation = DeltaRotation * currentQuat;
    newRotation.Normalize ();

    if (!Interpolate)
        {
        RootComponent->SetRotation ( newRotation );
        return;
        }

        // Интерполированное вращение
    TargetRotation = newRotation;
    LerpStartRotation = currentQuat;
    RotationLerpAlpha = 0.0f;
    bIsLerpingRotation = true;
    // Не сбрасываем bIsLerpingLocation
    }

void CActor::MoveActorInDirection ( const FVector & Direction, float Distance, bool Interpolate )
    {
    if (!RootComponent || Direction.IsZero ())
        return;

    // Нормализуем направление и умножаем на расстояние
    FVector normalizedDir = Direction.Normalized ();
    FVector delta = normalizedDir * Distance;

    MoveActor ( delta, Interpolate );
    }

void CActor::RotateAroundAxis ( const FVector & Axis, float AngleDegrees, bool Interpolate )
    {
    if (!RootComponent || Axis.IsZero ())
        return;

    // Создаем кватернион вращения вокруг оси
    FQuat rotationQuat ( Axis.Normalized (), CEMath::DegreesToRadians ( AngleDegrees ) );

    AddActorLocalRotation ( rotationQuat, Interpolate );
    }

void CActor::SetActorName ( const std::string & newName )
    {
    this->Rename ( newName );
    }