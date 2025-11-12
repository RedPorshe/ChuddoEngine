#include "Engine/GamePlay/Actors/Character.h"

#include "Engine/Core/CollisionSystem.h"
#include "Engine/GamePlay/Components/PlaneComponent.h"

namespace CE
{
  CECharacter::CECharacter(CEObject* Owner, FString NewName)
      : CEPawn(Owner, NewName), m_VerticalVelocity(0.0f), m_IsOnGround(false)
  {
    SetupCharacterComponents();
    m_IsOnGround = false;
  }

  void CECharacter::SetupCharacterComponents()
  {
    // Создаем капсульную коллизию как корневой компонент
    m_CapsuleComponent = AddDefaultSubObject<CECapsuleComponent>("Collision", this, "CapsuleComponent");
    m_CapsuleComponent->SetCollisionEnabled(true);
    m_CapsuleComponent->SetGenerateOverlapEvents(true);
    // Устанавливаем капсулу как корневой компонент
    SetRootComponent(m_CapsuleComponent);

    // Настраиваем размеры капсулы (в метрах)
    m_CapsuleComponent->SetRadius(0.35f);  // 35 см радиус
    m_CapsuleComponent->SetHeight(1.8f);   // 1.8 м высота

    // Создаем меш компонент и аттачим к капсуле
    m_MeshComponent = AddDefaultSubObject<MeshComponent>("Mesh", this, "MeshComponent");
    m_MeshComponent->AttachToComponent(m_CapsuleComponent);
    m_MeshComponent->SetPosition(0.0f, 0.0f, 0.0f);  // Центр капсулы
    m_MeshComponent->SetScale(0.7f, 1.8f, 0.7f);     // Масштабируем под размер капсулы

    // Создаем SpringArm компонент
    m_SpringArm = AddDefaultSubObject<SpringArmComponent>("SpringArm", this, "SpringArmComponent");
    m_SpringArm->SetTargetOffset(glm::vec3(0.0f, 0.9f, 0.0f));  // Смещение к "голове" (90 см ВВЕРХ)
    m_SpringArm->SetArmLength(5.0f);                            // 5 метров от персонажа
    m_SpringArm->SetCameraLag(0.05f);                           // Небольшая задержка для плавности
    m_SpringArm->AttachToComponent(m_CapsuleComponent);

    // Создаем камеру и аттачим к SpringArm
    m_Camera = AddDefaultSubObject<CameraComponent>("Camera", this, "CameraComponent");
    m_Camera->AttachToComponent(m_SpringArm);

    // Настраиваем камеру
    m_Camera->SetPosition(0.0f, 0.0f, 0.0f);  // Позиция относительно SpringArm
    m_Camera->SetFieldOfView(60.0f);
    m_Camera->SetAspectRatio(16.0f / 9.0f);
    m_Camera->SetNearPlane(0.1f);   // 10 см
    m_Camera->SetFarPlane(100.0f);  // 100 метров

    CE_CORE_DEBUG("Character components setup complete for: ", GetName());
  }

  void CECharacter::BeginPlay()
  {
    CEPawn::BeginPlay();

    // Дополнительная настройка при начале игры
    if (m_MeshComponent)
    {
      m_MeshComponent->SetColor(glm::vec3(0.2f, 0.6f, 1.0f));  // Синий цвет для персонажа
    }
  }

  void CECharacter::Tick(float DeltaTime)
  {
    CEPawn::Tick(DeltaTime);

    const float gravity = -9.8f;

    if (m_CapsuleComponent)
    {
      auto collisions = CollisionSystem::Get().CheckCollisions(m_CapsuleComponent);

      m_IsOnGround = false;

      for (const auto& hit : collisions)
      {
        if (hit.HitComponent && hit.bBlockingHit)
        {
          m_IsOnGround = true;

          break;
        }
      }

      // Применяем гравитацию только если не на земле
      if (!m_IsOnGround)
      {
        m_VerticalVelocity += gravity * DeltaTime;
      }
      else
      {
        // На земле - обнуляем скорость падения
        if (m_VerticalVelocity < 0.0f)
        {
          m_VerticalVelocity = 0.0f;
        }
      }

      // Двигаем по вертикали
      if (abs(m_VerticalVelocity) > 0.01f)
      {
        glm::vec3 movement(0.0f, m_VerticalVelocity * DeltaTime, 0.0f);
        m_CapsuleComponent->Move(movement);
      }

      if (GetActorLocation().y < -10.0f)
      {
        SetActorLocation(glm::vec3(0.0f, 5.0f, 5.0f));
        m_VerticalVelocity = 0.0f;
      }
    }
  }

  void CECharacter::SetupPlayerInputComponent()
  {
    CEPawn::SetupPlayerInputComponent();

    if (GetInputComponent())
    {
      // Биндинги управления для персонажа
      GetInputComponent()->BindAxis("MoveForward", [this](float Value)
                                    {
        if (m_CapsuleComponent && abs(Value) > 0.1f)
        {
          glm::vec3 movement = glm::vec3(0.0f, 0.0f, Value * .020f); // Движение вперед/назад
          m_CapsuleComponent->Move(movement);
        } });

      GetInputComponent()->BindAxis("MoveRight", [this](float Value)
                                    {
        if (m_CapsuleComponent && abs(Value) > 0.1f)
        {
          glm::vec3 movement = glm::vec3(Value * .020f, 0.0f, 0.0f); // Движение вправо/влево
          m_CapsuleComponent->Move(movement);
        } });

      GetInputComponent()->BindAction("Jump", EInputEvent::Pressed, [this]()
                                      {
        if (m_IsOnGround)
        {
          // Прыжок вверх (положительное значение Y)
          m_VerticalVelocity = 4.0f; // Импульс вверх
          CE_CORE_DEBUG("JUMP! Vertical velocity: ", m_VerticalVelocity);
        } });

      CE_CORE_DEBUG("Character input bindings setup complete");
    }
  }
}  // namespace CE