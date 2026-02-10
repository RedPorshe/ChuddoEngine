#include "Actors/Actor.h"
#include "World/Level.h"
#include "World/World.h"
#include "GameInstance.h"
#include "GameFramework/Components/BaseComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BaseCollisionComponent.h"


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
            LOG_DEBUG ( "[ACTOR] Location lerp completed for: ", GetName (),
                        " final position=(", TargetLocation.x, ", ",
                        TargetLocation.y, ", ", TargetLocation.z, ")" );
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
            LOG_DEBUG ( "[ACTOR] Rotation lerp completed for: ", GetName () );
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

    // ============================================================================
    // Direction vectors getters
    // ============================================================================

FVector CActor::GetActorForwardVector ()
    {
    if (!RootComponent) return FVector::Forward ();
    FQuat rotation = RootComponent->GetRotationQuat ();
    rotation.Normalize ();
    // Вперед по оси Z (0, 0, 1) в локальном пространстве
    return rotation * FVector::Forward ();
    }

FVector CActor::GetActorRightVector ()
    {
    if (!RootComponent) return FVector::Right ();
    FQuat rotation = RootComponent->GetRotationQuat ();
    rotation.Normalize ();
    // Вправо по оси X (1, 0, 0) в локальном пространстве
    return rotation * FVector::Right ();
    }

FVector CActor::GetActorUpVector ()
    {
    if (!RootComponent) return FVector::Up ();
    FQuat rotation = RootComponent->GetRotationQuat ();
    rotation.Normalize ();
    // Вверх по оси Y (0, 1, 0) в локальном пространстве
    return rotation * FVector::Up ();
    }

    // ============================================================================
    // Transform setters
    // ============================================================================

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

    // ============================================================================
    // Teleport functions (immediate movement)
    // ============================================================================

void CActor::TeleportTo ( const FVector & NewLocation )
    {
    LOG_DEBUG ( "[ACTOR] TeleportTo: ", GetName (), " to (",
                NewLocation.x, ", ", NewLocation.y, ", ", NewLocation.z, ")" );
    SetActorLocation ( NewLocation );
    }

void CActor::TeleportTo ( float NewX, float NewY, float NewZ )
    {
    TeleportTo ( FVector ( NewX, NewY, NewZ ) );
    }

void CActor::SetActorRotationImmediately ( const FQuat & NewRotation )
    {
    LOG_DEBUG ( "[ACTOR] SetActorRotationImmediately: ", GetName () );
    SetActorRotation ( NewRotation );
    }

void CActor::SetActorRotationImmediately ( const FVector & NewRotation )
    {
    LOG_DEBUG ( "[ACTOR] SetActorRotationImmediately: ", GetName (),
                " to (", NewRotation.x, ", ", NewRotation.y, ", ", NewRotation.z, ")" );
    SetActorRotation ( NewRotation );
    }

void CActor::SetActorRotationImmediately ( float inX, float inY, float inZ )
    {
    SetActorRotationImmediately ( FVector ( inX, inY, inZ ) );
    }

    // ============================================================================
    // Movement methods
    // ============================================================================

void CActor::MoveActor ( const FVector & Delta, bool Interpolate )
    {
    if (!RootComponent)
        return;

    LOG_DEBUG ( "[ACTOR] MoveActor called for: ", GetName (),
                " Delta=(", Delta.x, ", ", Delta.y, ", ", Delta.z, ")",
                " Interpolate=", Interpolate );

    if (!Interpolate)
        {
            // Мгновенное перемещение
        FVector currentLocation = RootComponent->GetLocation ();
        RootComponent->SetLocation ( currentLocation + Delta );
        LOG_DEBUG ( "[ACTOR] Instant move to: (",
                    currentLocation.x + Delta.x, ", ",
                    currentLocation.y + Delta.y, ", ",
                    currentLocation.z + Delta.z, ")" );
        return;
        }

        // Интерполированное перемещение
    FVector currentLocation = RootComponent->GetLocation ();
    TargetLocation = currentLocation + Delta;
    LerpStartLocation = currentLocation;
    LocationLerpAlpha = 0.0f;
    bIsLerpingLocation = true;

    LOG_DEBUG ( "[ACTOR] Starting location lerp from: (",
                currentLocation.x, ", ", currentLocation.y, ", ", currentLocation.z, ")",
                " to: (", TargetLocation.x, ", ", TargetLocation.y, ", ", TargetLocation.z, ")",
                " LerpSpeed=", LerpSpeed );
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

    RotateActor ( deltaQuat, Interpolate );
    }

void CActor::RotateActor ( const FQuat & DeltaRotation, bool Interpolate )
    {
    if (!RootComponent)
        return;

    LOG_DEBUG ( "[ACTOR] RotateActor called for: ", GetName (),
                " Interpolate=", Interpolate );

      // Получаем текущее вращение
    FQuat currentQuat = RootComponent->GetRotationQuat ();

    // Вычисляем целевое вращение (УМНОЖАЕМ СПРАВА для локального вращения)
    FQuat targetQuat = currentQuat * DeltaRotation;
    targetQuat.Normalize ();

    if (!Interpolate)
        {
            // Мгновенное вращение
        RootComponent->SetRotation ( targetQuat );
        LOG_DEBUG ( "[ACTOR] Instant rotation to new quaternion" );
        return;
        }

        // Интерполированное вращение
    TargetRotation = targetQuat;
    LerpStartRotation = currentQuat;
    RotationLerpAlpha = 0.0f;
    bIsLerpingRotation = true;

    LOG_DEBUG ( "[ACTOR] Starting rotation lerp" );
    }

    // ============================================================================
    // Unreal Engine style input functions (ИСПРАВЛЕННЫЕ!)
    // ============================================================================

//void CActor::AddMovementInput ( const FVector & WorldDirection, float ScaleValue, bool Interpolate )
//    {
//    if (!RootComponent || WorldDirection.IsZero () || ScaleValue == 0.0f)
//        return;
//
//    LOG_DEBUG ( "[ACTOR] AddMovementInput: ", GetName (),
//                " Direction=(", WorldDirection.x, ", ", WorldDirection.y, ", ", WorldDirection.z, ")",
//                " ScaleValue=", ScaleValue, " Interpolate=", Interpolate );
//
//      // Нормализуем направление и масштабируем
//    FVector normalizedDir = WorldDirection.Normalized ();
//    FVector delta = normalizedDir * ScaleValue;
//
//    MoveActor ( delta, Interpolate );
//    }

//void CActor::AddControllerYawInput ( float YawDegrees, bool Interpolate )
//    {
//    if (!RootComponent || YawDegrees == 0.0f)
//        return;
//
//    LOG_DEBUG ( "[ACTOR] AddControllerYawInput: ", GetName (),
//                " YawDegrees=", YawDegrees, " Interpolate=", Interpolate );
//
//      // Вращение по оси Y (yaw) - ВОКРУГ МИРОВОЙ ОСИ Y (0, 1, 0)
//    FQuat yawRotation = FQuat ( FVector::Up (), CEMath::DegreesToRadians ( YawDegrees ) );
//    AddActorWorldRotation ( yawRotation, Interpolate );
//    }

//void CActor::AddControllerPitchInput ( float PitchDegrees, bool Interpolate )
//    {
//    if (!RootComponent || PitchDegrees == 0.0f)
//        return;
//
//    LOG_DEBUG ( "[ACTOR] AddControllerPitchInput: ", GetName (),
//                " PitchDegrees=", PitchDegrees, " Interpolate=", Interpolate );
//
//      // Вращение по оси X (pitch) - ВОКРУГ МИРОВОЙ ОСИ X (1, 0, 0)
//      // Но сначала нужно получить текущий вектор right в мировых координатах
//    FQuat currentRotation = GetActorRotationQuat ();
//    FVector worldRight = currentRotation * FVector::Right ();
//
//    FQuat pitchRotation = FQuat ( worldRight, CEMath::DegreesToRadians ( PitchDegrees ) );
//    AddActorWorldRotation ( pitchRotation, Interpolate );
//    }

//void CActor::AddControllerRollInput ( float RollDegrees, bool Interpolate )
//    {
//    if (!RootComponent || RollDegrees == 0.0f)
//        return;
//
//    LOG_DEBUG ( "[ACTOR] AddControllerRollInput: ", GetName (),
//                " RollDegrees=", RollDegrees, " Interpolate=", Interpolate );
//
//      // Вращение по оси Z (roll) - ВОКРУГ МИРОВОЙ ОСИ Z (0, 0, 1)
//      // Но сначала нужно получить текущий вектор forward в мировых координатах
//    FQuat currentRotation = GetActorRotationQuat ();
//    FVector worldForward = currentRotation * FVector::Forward ();
//
//    FQuat rollRotation = FQuat ( worldForward, CEMath::DegreesToRadians ( RollDegrees ) );
//    AddActorWorldRotation ( rollRotation, Interpolate );
//    }

//void CActor::AddActorLocalRotationInput ( const FVector & RotationDegrees, bool Interpolate )
//    {
//    if (!RootComponent || RotationDegrees.IsZero ())
//        return;
//
//    LOG_DEBUG ( "[ACTOR] AddActorLocalRotationInput: ", GetName (),
//                " RotationDegrees=(", RotationDegrees.x, ", ", RotationDegrees.y, ", ", RotationDegrees.z, ")",
//                " Interpolate=", Interpolate );
//
//      // Создаем кватернион вращения для каждого угла (локальное вращение)
//    FQuat rotation = FQuat::FromEulerAngles (
//        CEMath::DegreesToRadians ( RotationDegrees.x ), // Pitch (вокруг X)
//        CEMath::DegreesToRadians ( RotationDegrees.y ), // Yaw (вокруг Y)
//        CEMath::DegreesToRadians ( RotationDegrees.z )  // Roll (вокруг Z)
//    );
//
//    AddActorLocalRotation ( rotation, Interpolate );
//    }

//void CActor::AddActorWorldRotationInput ( const FVector & RotationDegrees, bool Interpolate )
//    {
//    if (!RootComponent || RotationDegrees.IsZero ())
//        return;
//
//    LOG_DEBUG ( "[ACTOR] AddActorWorldRotationInput: ", GetName (),
//                " RotationDegrees=(", RotationDegrees.x, ", ", RotationDegrees.y, ", ", RotationDegrees.z, ")",
//                " Interpolate=", Interpolate );
//
//      // Создаем кватернион вращения для каждого угла (мировое вращение)
//    FQuat rotation = FQuat::FromEulerAngles (
//        CEMath::DegreesToRadians ( RotationDegrees.x ), // Pitch (вокруг мирового X)
//        CEMath::DegreesToRadians ( RotationDegrees.y ), // Yaw (вокруг мирового Y)
//        CEMath::DegreesToRadians ( RotationDegrees.z )  // Roll (вокруг мирового Z)
//    );
//
//    AddActorWorldRotation ( rotation, Interpolate );
//    }

    // ============================================================================
    // Offset methods (world and local space) (ИСПРАВЛЕННЫЕ!)
    // ============================================================================

void CActor::AddActorWorldOffset ( const FVector & DeltaLocation, bool Interpolate )
    {
        // Мировое смещение - просто добавляем к мировой позиции
    MoveActor ( DeltaLocation, Interpolate );
    }

void CActor::AddActorLocalOffset ( const FVector & DeltaLocation, bool Interpolate )
    {
    if (!RootComponent)
        return;

    LOG_DEBUG ( "[ACTOR] AddActorLocalOffset: ", GetName (),
                " DeltaLocation=(", DeltaLocation.x, ", ", DeltaLocation.y, ", ", DeltaLocation.z, ")",
                " Interpolate=", Interpolate );

      // Локальное смещение: учитываем вращение актора
    FQuat rotation = RootComponent->GetRotationQuat ();
    rotation.Normalize ();

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

    // МИРОВОЕ вращение: умножаем СПРАВА (q' = q * Δq)
    FQuat newRotation = currentQuat * DeltaRotation;
    newRotation.Normalize ();

    if (!Interpolate)
        {
        RootComponent->SetRotation ( newRotation );
        LOG_DEBUG ( "[ACTOR] Instant world rotation" );
        return;
        }

        // Интерполированное вращение
    TargetRotation = newRotation;
    LerpStartRotation = currentQuat;
    RotationLerpAlpha = 0.0f;
    bIsLerpingRotation = true;

    LOG_DEBUG ( "[ACTOR] Starting world rotation lerp" );
    }

void CActor::AddActorLocalRotation ( const FQuat & DeltaRotation, bool Interpolate )
    {
    if (!RootComponent)
        return;

    // Получаем текущее вращение
    FQuat currentQuat = RootComponent->GetRotationQuat ();

    // ЛОКАЛЬНОЕ вращение: умножаем СЛЕВА (q' = Δq * q)
    FQuat newRotation = DeltaRotation * currentQuat;
    newRotation.Normalize ();

    if (!Interpolate)
        {
        RootComponent->SetRotation ( newRotation );
        LOG_DEBUG ( "[ACTOR] Instant local rotation" );
        return;
        }

        // Интерполированное вращение
    TargetRotation = newRotation;
    LerpStartRotation = currentQuat;
    RotationLerpAlpha = 0.0f;
    bIsLerpingRotation = true;

    LOG_DEBUG ( "[ACTOR] Starting local rotation lerp" );
    }

    // ============================================================================
    // Helper movement methods
    // ============================================================================

void CActor::MoveActorInDirection ( const FVector & Direction, float Distance, bool Interpolate )
    {
    if (!RootComponent || Direction.IsZero ())
        return;

    LOG_DEBUG ( "[ACTOR] MoveActorInDirection: ", GetName (),
                " Direction=(", Direction.x, ", ", Direction.y, ", ", Direction.z, ")",
                " Distance=", Distance, " Interpolate=", Interpolate );

      // Нормализуем направление и умножаем на расстояние
    FVector normalizedDir = Direction.Normalized ();
    FVector delta = normalizedDir * Distance;

    MoveActor ( delta, Interpolate );
    }

void CActor::RotateAroundAxis ( const FVector & Axis, float AngleDegrees, bool Interpolate )
    {
    if (!RootComponent || Axis.IsZero ())
        return;

    LOG_DEBUG ( "[ACTOR] RotateAroundAxis: ", GetName (),
                " Axis=(", Axis.x, ", ", Axis.y, ", ", Axis.z, ")",
                " AngleDegrees=", AngleDegrees, " Interpolate=", Interpolate );

      // Создаем кватернион вращения вокруг оси
    FQuat rotationQuat ( Axis.Normalized (), CEMath::DegreesToRadians ( AngleDegrees ) );

    AddActorLocalRotation ( rotationQuat, Interpolate );
    }

    // ============================================================================
    // Collision methods
    // ============================================================================

void CActor::SetCollisionEnabled ( bool value )
    {
    bIsCollisionEnabled = value;
    if (RootComponent)
        {
        RootComponent->SetCollisionEnabled ( bIsCollisionEnabled );
        }
    }

void CActor::OnComponentBeginOverlap ( CBaseCollisionComponent * other )
    {
    if (other == nullptr) return;

    LOG_DEBUG ( "OnComponentBeginOverlap with : ", other->GetOwnerActor ()->GetName (), " for ", GetName () );
    LOG_DEBUG ( "stub implementation TODO: implement real implementation" );
    }

void CActor::OnComponentEndOverlap ( CBaseCollisionComponent * other )
    {
    if (other == nullptr) return;

    LOG_DEBUG ( "OnComponentEndOverlap with : ", other->GetOwnerActor ()->GetName (), " for ", GetName () );
    LOG_DEBUG ( "stub implementation TODO: implement real implementation" );
    }

void CActor::SetActorName ( const std::string & newName )
    {
    this->Rename ( newName );
    }