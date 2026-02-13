#include "TestCubeActor.h"
#include "Components/Collisions/BaseCollisionComponent.h"
#include "Components/Collisions/CapsuleComponent.h"
#include "Components/Collisions/BoxComponent.h"
#include "Components/Collisions/SphereComponent.h"
#include "Core/CollisionSystem.h"

CTestCube::CTestCube ( CObject * inOwner, const std::string InName ) :Super ( inOwner, InName )
	{
	if (GetRootComponent ())
		{
		GetRootComponent ()->SetAutoGenerateCollisionComponent ( false );
		}

	auto BoxCollision = AddDefaultSubObject<CCapsuleComponent> ( "Capsule" );
	if (BoxCollision)
		{
			// Устанавливаем размер 50x50x50 (половина размеров 25,25,25)
		BoxCollision->SetHalfHeight ( 50.f );
		BoxCollision->SetCollisionEnabled ( true );

		// Не устанавливаем канал здесь - установим в Level
		// BoxCollision->SetChannelAsDynamic(); // Убираем отсюда

		if (GetRootComponent ())
			{
			GetRootComponent ()->SetCollisionComponent ( BoxCollision );
			}

		LOG_DEBUG ( "[TESTCUBE] Created BoxCollision: ", BoxCollision->GetName () );
		}
	}

CTestCube::~CTestCube ()
	{}

void CTestCube::BeginPlay ()
	{
	Super::BeginPlay ();
	}

void CTestCube::Tick ( float deltaTime )
	{
	Super::Tick ( deltaTime );
	}

void CTestCube::EndPlay ()
	{
	Super::EndPlay ();
	}

void CTestCube::OnComponentHit ( CBaseCollisionComponent * other )
	{
	Super::OnComponentHit ( other );
	bIsCanMove = false;
	}

CTestSphere::CTestSphere ( CObject * inOwner, const std::string InName ) :Super ( inOwner, InName )
	{
	 // 1. ЯВНО отключаем авто-генерацию коллизионного компонента
	if (GetRootComponent ())
		{
		GetRootComponent ()->SetAutoGenerateCollisionComponent ( false );
		}

		// 2. СОЗДАЕМ свой SphereComponent
	CSphereComponent * SphereCollision = AddDefaultSubObject<CSphereComponent> ( "SphereCollision" );
	if (SphereCollision)
		{
			// Настраиваем радиус
		SphereCollision->SetRadius ( 50.0f );
		SphereCollision->SetCollisionEnabled ( true );

		// ВАЖНО: Устанавливаем канал как Dynamic
		SphereCollision->SetChannelAsDynamic ();



		// 3. Привязываем к корневому компоненту
		GetRootComponent ()->SetCollisionComponent ( SphereCollision );

		LOG_DEBUG ( "[TESTSPHERE] Created OWN SphereCollision: ", SphereCollision->GetName (),
					", Channel: ", SphereCollision->GetCollisionChannel ().GetName () );
		}
	}

CTestSphere::~CTestSphere ()
	{}

void CTestSphere::BeginPlay ()
	{
	Super::BeginPlay ();
	}

void CTestSphere::Tick ( float deltaTime )
	{
	Super::Tick ( deltaTime );

	
	}


void CTestSphere::EndPlay ()
	{
	Super::EndPlay ();
	}

void CTestSphere::OnComponentHit ( CBaseCollisionComponent * other )
	{
	Super::OnComponentHit ( other );
	bIsCanMove = false;
	}

CBaseCollisionComponent * CTestSphere::GetCollisionComponent ()
	{
	if (GetRootComponent ())
		{
		if (auto ret = GetRootComponent ()->GetCollisionComponent ())
			return ret;
		}
	for (auto comp : ActorComponents)
		{
		if (comp != nullptr)
			{
			if (CBaseCollisionComponent * toreturn = dynamic_cast< CBaseCollisionComponent * >( comp ))
				{
				return toreturn;
				}
			}
		}
	return nullptr;
	}
