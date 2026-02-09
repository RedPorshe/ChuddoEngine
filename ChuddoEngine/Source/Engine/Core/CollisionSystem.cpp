#include "Core/CollisionSystem.h"
#include "Actors/Actor.h"
#include "Components/TransformComponent.h"
#include "Components/SphereComponent.h"
#include <algorithm>
#include <cmath>
#include <limits>



// Определяем статические члены
std::unique_ptr<CCollisionSystem> CCollisionSystem::s_Instance = nullptr;
bool CCollisionSystem::s_IsInitialized = false;


// Конструктор должен быть публичным для фабрики объектов
CCollisionSystem::CCollisionSystem ( CObject * inOwner, const std::string & inDisplayName )
	: CObject ( inOwner, inDisplayName )
	{
	LOG_DEBUG ( "Collision System created: ", inDisplayName );
	}

CCollisionSystem::~CCollisionSystem ()
	{
	m_CollisionComponents.clear ();
	m_SpatialGrid.clear ();
	m_Callbacks.clear ();
	m_LastPositions.clear ();

	LOG_DEBUG ( "Collision System destroyed" );
	}

CCollisionSystem & CCollisionSystem::Get ()
	{
	if (!s_Instance)
		{
			// Создаем синглтон без владельца
		s_Instance = std::make_unique<CCollisionSystem> ( nullptr, "GlobalCollisionSystem" );
		s_IsInitialized = true;
		}
	return *s_Instance;
	}

void CCollisionSystem::RegisterCollisionComponent ( CBaseCollisionComponent * component )
	{
	if (!component)
		return;

	auto it = std::find ( m_CollisionComponents.begin (),
						  m_CollisionComponents.end (), component );

	if (it == m_CollisionComponents.end ())
		{
		m_CollisionComponents.push_back ( component );

		// Сохраняем начальную позицию
		if (component->GetOwnerActor ())
			{
			m_LastPositions[ component ] = component->GetOwnerActor ()->GetActorLocation ();
			}

		LOG_DEBUG ( "Registered collision component: ", component->GetName () );
		}
	}

void CCollisionSystem::UnregisterCollisionComponent ( CBaseCollisionComponent * component )
	{
	if (!component)
		return;

	auto it = std::find ( m_CollisionComponents.begin (),
						  m_CollisionComponents.end (), component );

	if (it != m_CollisionComponents.end ())
		{
		m_CollisionComponents.erase ( it );
		m_LastPositions.erase ( component );

		// Удаляем из всех overlapped компонентов
		for (auto & other : m_CollisionComponents)
			{
			if (other != component)
				{
					// Вызываем OnEndOverlap для всех компонентов, которые перекрывались
				if (component->ShouldOverlapWith ( other ))
					{
					other->OnEndOverlap ( component );
					}
				}
			}

		LOG_DEBUG ( "Unregistered collision component: ", component->GetName () );
		}
	}

void CCollisionSystem::Update ( float deltaTime )
	{
		// Обновляем с заданной частотой
	m_AccumulatedTime += deltaTime;
	float updateInterval = 1.0f / m_UpdateRate;

	if (m_AccumulatedTime >= updateInterval)
		{
		ProcessCollisions ();
		m_AccumulatedTime -= updateInterval;
		}
	}

void CCollisionSystem::ProcessCollisions ()
	{
	m_LastFrameCollisions = 0;

	LOG_DEBUG ( "[COLLISION] ======= START FRAME =======" );
	LOG_DEBUG ( "[COLLISION] Processing ", m_CollisionComponents.size (), " collision components" );

	// Выведем все компоненты для отладки
	for (size_t i = 0; i < m_CollisionComponents.size (); ++i)
		{
		CBaseCollisionComponent * comp = m_CollisionComponents[ i ];
		if (comp)
			{
			FVector pos = comp->GetWorldLocation ();
			LOG_DEBUG ( "[COLLISION] Component ", i, ": ", comp->GetName (),
						" at (", pos.x, ", ", pos.y, ", ", pos.z, ")",
						" Shape: ", Collision::ShapeToString ( comp->GetShapeType () ),
						" Enabled: ", comp->IsCollisionEnabled () );
			}
		}

		// Обрабатываем все потенциальные пары
	for (size_t i = 0; i < m_CollisionComponents.size (); ++i)
		{
		CBaseCollisionComponent * compA = m_CollisionComponents[ i ];

		if (!compA)
			{
			LOG_DEBUG ( "[COLLISION] Component A is null at index ", i );
			continue;
			}

		if (!compA->IsCollisionEnabled ())
			{
			LOG_DEBUG ( "[COLLISION] Component A '", compA->GetName (), "' is disabled" );
			continue;
			}

		LOG_DEBUG ( "[COLLISION] --- Checking pairs for ", compA->GetName (), " ---" );

		// Проверяем со всеми компонентами после текущего
		for (size_t j = i + 1; j < m_CollisionComponents.size (); ++j)
			{
			CBaseCollisionComponent * compB = m_CollisionComponents[ j ];

			if (!compB)
				{
				LOG_DEBUG ( "[COLLISION]   Component B is null at index ", j );
				continue;
				}

			if (!compB->IsCollisionEnabled ())
				{
				LOG_DEBUG ( "[COLLISION]   Component B '", compB->GetName (), "' is disabled" );
				continue;
				}

			LOG_DEBUG ( "[COLLISION]   Pair: ", compA->GetName (), " vs ", compB->GetName () );

			// Проверяем, могут ли они коллизировать по каналам
			bool canCollideA = compA->CanCollideWith ( compB );
			bool canCollideB = compB->CanCollideWith ( compA );

			LOG_DEBUG ( "[COLLISION]     Can collide A->B: ", canCollideA, ", B->A: ", canCollideB );

			if (!canCollideA || !canCollideB)
				{
				LOG_DEBUG ( "[COLLISION]     Skipping - cannot collide (channel mismatch)" );
				continue;
				}

				// ПРОВЕРЯЕМ ТОЛЬКО СФЕРЫ (упрощенный вариант)
			ECollisionShape shapeA = compA->GetShapeType ();
			ECollisionShape shapeB = compB->GetShapeType ();

			LOG_DEBUG ( "[COLLISION]     Shape A: ", Collision::ShapeToString ( shapeA ),
						", Shape B: ", Collision::ShapeToString ( shapeB ) );

			   // Если хотя бы один компонент не сфера - пропускаем (для упрощения)
			if (shapeA != ECollisionShape::SPHERE || shapeB != ECollisionShape::SPHERE)
				{
				LOG_DEBUG ( "[COLLISION]     Skipping - not both spheres" );
				continue;
				}

			LOG_DEBUG ( "[COLLISION]     Both are spheres - checking collision..." );

			FCollisionInfo collisionInfo;
			collisionInfo.ComponentA = compA;
			collisionInfo.ComponentB = compB;

			// Проверка сфера-сфера
			bool bCollision = CheckSphereSphere ( compA, compB, collisionInfo );

			if (bCollision)
				{
				m_LastFrameCollisions++;
				LOG_DEBUG ( "[COLLISION]     *** COLLISION DETECTED! *** Depth: ", collisionInfo.Depth );

				// Определяем тип реакции
				bool bShouldBlock = compA->ShouldBlockWith ( compB ) || compB->ShouldBlockWith ( compA );
				bool bShouldOverlap = compA->ShouldOverlapWith ( compB ) && compB->ShouldOverlapWith ( compA );

				LOG_DEBUG ( "[COLLISION]     Should block: ", bShouldBlock, ", Should overlap: ", bShouldOverlap );

				if (bShouldOverlap)
					{
					LOG_DEBUG ( "[COLLISION]     Triggering overlap event" );
					// Вызываем события overlap
					compA->OnBeginOverlap ( compB );

					// Отправляем событие
					FireCollisionEvent ( ECollisionEventType::BEGIN_OVERLAP, collisionInfo );
					}
				else if (bShouldBlock)
					{
					LOG_DEBUG ( "[COLLISION]     Triggering block collision" );
					// Разрешаем столкновение
					ResolveCollision ( collisionInfo );

					// Отправляем событие
					FireCollisionEvent ( ECollisionEventType::COLLISION_HIT, collisionInfo );
					}
				}
			else
				{
				LOG_DEBUG ( "[COLLISION]     No collision" );
				}
			}
		}

	LOG_DEBUG ( "[COLLISION] ======= END FRAME =======" );
	LOG_DEBUG ( "[COLLISION] Total collisions this frame: ", m_LastFrameCollisions );
	}


	bool CCollisionSystem::CheckSphereSphere ( CBaseCollisionComponent * a,
											   CBaseCollisionComponent * b,
											   FCollisionInfo & outInfo ) const
		{
		if (!a || !b)
			{
			LOG_DEBUG ( "[SPHERE-SPHERE] ERROR: null components" );
			return false;
			}

		FVector posA = a->GetWorldLocation ();
		FVector posB = b->GetWorldLocation ();

		LOG_DEBUG ( "[SPHERE-SPHERE] Checking: ", a->GetName (), " vs ", b->GetName () );
		LOG_DEBUG ( "[SPHERE-SPHERE]   Position A: (", posA.x, ", ", posA.y, ", ", posA.z, ")" );
		LOG_DEBUG ( "[SPHERE-SPHERE]   Position B: (", posB.x, ", ", posB.y, ", ", posB.z, ")" );

		float radiusA = a->GetCollisionRadius(); // Временное значение по умолчанию
		float radiusB = b->GetCollisionRadius ();

		// Пробуем получить радиус из компонента, если это CTestSphereCollisionComponent
		if (auto * testComp = dynamic_cast< CSphereComponent * >( a ))
			{
			radiusA = testComp->GetRadius ();
			LOG_DEBUG ( "[SPHERE-SPHERE]   Radius A (from CSphereComponent): ", radiusA );
			}
		else
			{
			LOG_DEBUG ( "[SPHERE-SPHERE]   Radius A (default): ", radiusA );
			}

		if (auto * testComp = dynamic_cast< CSphereComponent * >( b ))
			{
			radiusB = testComp->GetRadius ();
			LOG_DEBUG ( "[SPHERE-SPHERE]   Radius B (from CSphereComponent): ", radiusB );
			}
		else
			{
			LOG_DEBUG ( "[SPHERE-SPHERE]   Radius B (default): ", radiusB );
			}

		FVector delta = posB - posA;
		float distanceSquared = delta.Dot ( delta );
		float distance = std::sqrt ( distanceSquared );
		float radiusSum = radiusA + radiusB;

		LOG_DEBUG ( "[SPHERE-SPHERE]   Distance: ", distance );
		LOG_DEBUG ( "[SPHERE-SPHERE]   Sum of radii: ", radiusSum );
		LOG_DEBUG ( "[SPHERE-SPHERE]   Collision? ", distance <= radiusSum ? "YES" : "NO" );
		LOG_DEBUG ( "[SPHERE-SPHERE]   Condition: distance (", distance, ") <= radiusSum (", radiusSum, ")" );

		if (distance <= radiusSum)
			{
			outInfo.Depth = radiusSum - distance;

			if (distance > 0.001f)
				{
				outInfo.Normal = delta / distance;
				outInfo.Location = posA + outInfo.Normal * ( radiusA - outInfo.Depth * 0.5f );
				}
			else
				{
				outInfo.Normal = FVector ( 0, 0, 1 );
				outInfo.Location = posA;
				}

			LOG_DEBUG ( "[SPHERE-SPHERE]   *** COLLISION FOUND ***" );
			LOG_DEBUG ( "[SPHERE-SPHERE]     Depth: ", outInfo.Depth );
			LOG_DEBUG ( "[SPHERE-SPHERE]     Normal: (", outInfo.Normal.x, ", ", outInfo.Normal.y, ", ", outInfo.Normal.z, ")" );
			LOG_DEBUG ( "[SPHERE-SPHERE]     Location: (", outInfo.Location.x, ", ", outInfo.Location.y, ", ", outInfo.Location.z, ")" );

			return true;
			}

		LOG_DEBUG ( "[SPHERE-SPHERE]   No collision" );
		return false;
		}


	bool CCollisionSystem::CheckSphereBox ( CBaseCollisionComponent * sphere, CBaseCollisionComponent * box, FCollisionInfo & outInfo ) const
		{
		LOG_DEBUG ( "[COLLISION] CheckSphereBox not implemented" );
		return false;
		}

	bool CCollisionSystem::CheckBoxBox ( CBaseCollisionComponent * a, CBaseCollisionComponent * b, FCollisionInfo & outInfo ) const
		{
		  // Заглушка - всегда возвращаем false для упрощения теста
		LOG_DEBUG ( "[COLLISION] CheckBoxBox not implemented" );
		return false;
		}

void CCollisionSystem::ResolveCollision ( const FCollisionInfo & collision )
	{
	if (!collision.ComponentA || !collision.ComponentB)
		return;

	// Простое разрешение столкновения - отталкиваем объекты
	CActor * actorA = collision.ComponentA->GetOwnerActor ();
	CActor * actorB = collision.ComponentB->GetOwnerActor ();

	if (actorA && actorB)
		{
		FVector push = collision.Normal * collision.Depth * 0.5f;

		// Сдвигаем оба объекта
		actorA->SetActorLocation ( actorA->GetActorLocation () - push );
		actorB->SetActorLocation ( actorB->GetActorLocation () + push );
		}
	}

void CCollisionSystem::FireCollisionEvent ( ECollisionEventType eventType, const FCollisionInfo & info )
	{
	auto it = m_Callbacks.find ( eventType );
	if (it != m_Callbacks.end ())
		{
		it->second ( info );
		}
	}

	// Raycasting методы
CCollisionSystem::FRaycastResult CCollisionSystem::Raycast ( const FVector & start,
															 const FVector & end,
															 const std::string & channelName ) const
	{
	FVector direction = end - start;
	float distance = direction.Length ();

	if (distance < 0.001f)
		return FRaycastResult ();

	direction = direction / distance;

	return Raycast ( start, direction, distance, channelName );
	}

CCollisionSystem::FRaycastResult CCollisionSystem::Raycast ( const FVector & start,
															 const FVector & direction,
															 float distance,
															 const std::string & channelName ) const
	{
	FRaycastResult result;
	float closestDistance = std::numeric_limits<float>::max ();

	FVector end = start + direction * distance;

	for (CBaseCollisionComponent * component : m_CollisionComponents)
		{
		if (!component || !component->IsCollisionEnabled ())
			continue;

		// Проверка канала
		if (channelName != "All")
			{
			if (!component->CanCollideWith ( channelName ))
				continue;
			}

		if (!component->GetOwnerActor ())
			continue;

		FVector compPos = component->GetOwnerActor ()->GetActorLocation ();
		float radius = component->GetCollisionRadius(); 

		// Простой ray-sphere тест
		FVector toSphere = compPos - start;
		float projection = toSphere.Dot ( direction );

		if (projection < 0.0f || projection > distance)
			continue;

		FVector closestPoint = start + direction * projection;
		FVector toClosest = closestPoint - compPos;
		float distanceSquared = toClosest.Dot ( toClosest );

		if (distanceSquared <= radius * radius)
			{
			float hitDistance = projection - std::sqrt ( radius * radius - distanceSquared );

			if (hitDistance >= 0.0f && hitDistance < closestDistance)
				{
				closestDistance = hitDistance;
				result.bHit = true;
				result.HitComponent = component;
				result.Location = start + direction * hitDistance;
				result.Normal = ( result.Location - compPos ).Normalized ();
				result.Distance = hitDistance;
				}
			}
		}

	return result;
	}

	// Sphere overlap
std::vector<CBaseCollisionComponent *> CCollisionSystem::SphereOverlap (
	const FVector & center, float radius, const std::string & channelName ) const
	{
	std::vector<CBaseCollisionComponent *> results;

	for (CBaseCollisionComponent * component : m_CollisionComponents)
		{
		if (!component || !component->IsCollisionEnabled ())
			continue;

		// Проверка канала
		if (channelName != "All")
			{
			if (!component->CanCollideWith ( channelName ))
				continue;
			}

		if (!component->GetOwnerActor ())
			continue;

		FVector compPos = component->GetOwnerActor ()->GetActorLocation ();
		float compRadius = component->GetCollisionRadius(); 

		FVector delta = compPos - center;
		float distanceSquared = delta.Dot ( delta );
		float radiusSum = radius + compRadius;

		if (distanceSquared <= radiusSum * radiusSum)
			{
			results.push_back ( component );
			}
		}

	return results;
	}

	// Box overlap
std::vector<CBaseCollisionComponent *> CCollisionSystem::BoxOverlap (
	const FVector & center, const FVector & halfExtents, const FVector & rotation,
	const std::string & channelName ) const
	{
		// Простая реализация - преобразуем в сферическую проверку
		// Для более точной проверки нужно реализовать SAT (Separating Axis Theorem)

	float sphereRadius = std::max ( halfExtents.x, std::max ( halfExtents.y, halfExtents.z ) );
	return SphereOverlap ( center, sphereRadius, channelName );
	}

	// Проверка столкновений для конкретного компонента
std::vector<FCollisionInfo> CCollisionSystem::CheckCollisions ( CBaseCollisionComponent * component ) const
	{
	std::vector<FCollisionInfo> collisions;

	if (!component || !component->IsCollisionEnabled ())
		return collisions;

	for (CBaseCollisionComponent * other : m_CollisionComponents)
		{
		if (!other || other == component || !other->IsCollisionEnabled ())
			continue;

		if (!component->CanCollideWith ( other ))
			continue;

		FCollisionInfo info;
		info.ComponentA = component;
		info.ComponentB = other;

		if (component->GetOwnerActor () && other->GetOwnerActor ())
			{
			FVector posA = component->GetOwnerActor ()->GetActorLocation ();
			FVector posB = other->GetOwnerActor ()->GetActorLocation ();

			float radiusA = component->GetCollisionRadius ();
			float radiusB = other->GetCollisionRadius ();
			float totalRadius = radiusA + radiusB;

			FVector delta = posB - posA;
			float distanceSquared = delta.Dot ( delta );
			float totalRadiusSquared = totalRadius * totalRadius;

			// Проверяем коллизию
			if (distanceSquared <= totalRadiusSquared)
				{
				float distance = std::sqrt ( distanceSquared );

				if (distance > 0.001f) // Избегаем деления на ноль
					{
					info.Normal = delta / distance;
					info.Depth = totalRadius - distance;
					// Точка контакта - середина между поверхностями
					info.Location = posA + info.Normal * ( radiusA - info.Depth * 0.5f );
					}
				else // Сферы почти в одной точке
					{
					info.Normal = FVector ( 0, 0, 1 ); // Стандартная нормаль
					info.Depth = totalRadius;
					info.Location = posA;
					}

				collisions.push_back ( info );
				}
			}
		}

	return collisions;
	}

	// Проверка столкновений в определенной области
std::vector<FCollisionInfo> CCollisionSystem::CheckCollisionsAtLocation ( const FVector & location, float radius ) const
	{
	std::vector<FCollisionInfo> collisions;

	auto components = SphereOverlap ( location, radius, "All" );

	for (CBaseCollisionComponent * comp : components)
		{
		if (!comp || !comp->GetOwnerActor ())
			continue;

		FCollisionInfo info;
		info.ComponentA = nullptr; // Отметка, что это проверка области
		info.ComponentB = comp;
		info.Location = comp->GetOwnerActor ()->GetActorLocation ();
		info.Normal = ( info.Location - location ).Normalized ();
		info.Depth = radius - ( info.Location - location ).Length ();

		collisions.push_back ( info );
		}

	return collisions;
	}

	// Callback методы
void CCollisionSystem::RegisterCollisionCallback ( ECollisionEventType eventType,
												   const FCollisionCallback & callback )
	{
	m_Callbacks[ eventType ] = callback;
	}

void CCollisionSystem::UnregisterCollisionCallback ( ECollisionEventType eventType )
	{
	m_Callbacks.erase ( eventType );
	}

	// Пространственное разделение
void CCollisionSystem::UpdateSpatialPartition ()
	{
	m_SpatialGrid.clear ();

	for (CBaseCollisionComponent * component : m_CollisionComponents)
		{
		if (!component || !component->GetOwnerActor ())
			continue;

		FVector position = component->GetOwnerActor ()->GetActorLocation ();

		// Вычисляем индекс ячейки
		int cellX = static_cast< int >( position.x / m_CellSize );
		int cellY = static_cast< int >( position.y / m_CellSize );
		int cellZ = static_cast< int >( position.z / m_CellSize );

		// Создаем ключ ячейки (можно использовать хэш)
		int64_t cellKey = ( static_cast< int64_t >( cellX ) << 42 ) |
			( static_cast< int64_t >( cellY ) << 21 ) |
			static_cast< int64_t >( cellZ );

		m_SpatialGrid[ cellKey ].Components.push_back ( component );
		}
	}

std::vector<CBaseCollisionComponent *> CCollisionSystem::GetPotentiallyCollidingComponents (
	CBaseCollisionComponent * component ) const
	{
	std::vector<CBaseCollisionComponent *> result;

	if (!component || !component->GetOwnerActor ())
		return result;

	FVector position = component->GetOwnerActor ()->GetActorLocation ();
	float radius = 100.0f; // TODO: Получить bounding радиус из компонента

	// Проверяем соседние ячейки
	int minCellX = static_cast< int >( ( position.x - radius ) / m_CellSize );
	int maxCellX = static_cast< int >( ( position.x + radius ) / m_CellSize );
	int minCellY = static_cast< int >( ( position.y - radius ) / m_CellSize );
	int maxCellY = static_cast< int >( ( position.y + radius ) / m_CellSize );
	int minCellZ = static_cast< int >( ( position.z - radius ) / m_CellSize );
	int maxCellZ = static_cast< int >( ( position.z + radius ) / m_CellSize );

	for (int x = minCellX; x <= maxCellX; ++x)
		{
		for (int y = minCellY; y <= maxCellY; ++y)
			{
			for (int z = minCellZ; z <= maxCellZ; ++z)
				{
				int64_t cellKey = ( static_cast< int64_t >( x ) << 42 ) |
					( static_cast< int64_t >( y ) << 21 ) |
					static_cast< int64_t >( z );

				auto it = m_SpatialGrid.find ( cellKey );
				if (it != m_SpatialGrid.end ())
					{
					for (CBaseCollisionComponent * other : it->second.Components)
						{
						if (other != component)
							{
							result.push_back ( other );
							}
						}
					}
				}
			}
		}

	return result;
	}