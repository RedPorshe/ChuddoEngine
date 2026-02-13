#include "Core/CollisionSystem.h"
#include "Actors/Actor.h"
#include "Components/TransformComponent.h"
#include "Components/Collisions/SphereComponent.h"
#include "Components/Collisions/CapsuleComponent.h"
#include "Components/GravityComponent.h"
#include "Components/Collisions/BoxComponent.h"
#include "Components/Collisions/TerrainComponent.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

// ============================================================================
// Static Member Initialization
// ============================================================================

std::unique_ptr<CCollisionSystem> CCollisionSystem::s_Instance = nullptr;
bool CCollisionSystem::s_IsInitialized = false;

// ============================================================================
// Constructors & Destructor
// ============================================================================

CCollisionSystem::CCollisionSystem ( CObject * inOwner, const std::string & inDisplayName )
	: CObject ( inOwner, inDisplayName )
	{
	LOG_DEBUG ( "Collision Initialized: ", inDisplayName );
	}

CCollisionSystem::~CCollisionSystem ()
	{
	m_CollisionComponents.clear ();
	m_SpatialGrid.clear ();
	m_Callbacks.clear ();
	m_LastPositions.clear ();
	m_PreviousFrameCollisions.clear ();
	m_CurrentFrameCollisions.clear ();

	LOG_DEBUG ( "Collision System destroyed" );
	}

	// ============================================================================
	// Singleton Access
	// ============================================================================

CCollisionSystem & CCollisionSystem::Get ()
	{
	if (!s_Instance)
		{
		s_Instance = std::make_unique<CCollisionSystem> ( nullptr, "GlobalCollisionSystem" );
		s_IsInitialized = true;
		}
	return *s_Instance;
	}

	// ============================================================================
	// Component Registration
	// ============================================================================

void CCollisionSystem::RegisterCollisionComponent ( CBaseCollisionComponent * component )
	{
	if (!component)
		return;

	auto it = std::find ( m_CollisionComponents.begin (), m_CollisionComponents.end (), component );

	if (it == m_CollisionComponents.end ())
		{
		m_CollisionComponents.push_back ( component );

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

	auto it = std::find ( m_CollisionComponents.begin (), m_CollisionComponents.end (), component );

	if (it != m_CollisionComponents.end ())
		{
		m_CollisionComponents.erase ( it );
		m_LastPositions.erase ( component );

		// Remove from collision tracking sets
		for (auto pairIt = m_PreviousFrameCollisions.begin (); pairIt != m_PreviousFrameCollisions.end ();)
			{
			if (pairIt->first == component || pairIt->second == component)
				pairIt = m_PreviousFrameCollisions.erase ( pairIt );
			else
				++pairIt;
			}

		for (auto pairIt = m_CurrentFrameCollisions.begin (); pairIt != m_CurrentFrameCollisions.end ();)
			{
			if (pairIt->first == component || pairIt->second == component)
				pairIt = m_CurrentFrameCollisions.erase ( pairIt );
			else
				++pairIt;
			}

		LOG_DEBUG ( "Unregistered collision component: ", component->GetName () );
		}
	}

	// ============================================================================
	// Main Update
	// ============================================================================

void CCollisionSystem::Update ( float deltaTime )
	{
	m_AccumulatedTime += deltaTime;
	m_AccumulatedTime2 += deltaTime;
	m_AccumulatedTime3 += deltaTime;
	m_AccumulatedTime4 += deltaTime;

	float updateInterval = 1.0f / m_UpdateRate;

	if (m_AccumulatedTime >= updateInterval)
		{
		ProcessCollisions ();
		m_AccumulatedTime -= updateInterval;
		}
	}

	// ============================================================================
	// Core Collision Processing
	// ============================================================================

void CCollisionSystem::ProcessCollisions ()
	{
	m_LastFrameCollisions = 0;

	// Меняем местами множества вместо очистки и вставки (эффективнее)
	std::swap ( m_PreviousFrameCollisions, m_CurrentFrameCollisions );
	m_CurrentFrameCollisions.clear ();

	// Оптимизация: используем пространственное разделение если включено
	if (bUseSpatialPartition && !m_SpatialGrid.empty ())
		{
		ProcessCollisionsSpatial ();
		}
	else
		{
			// Стандартный O(n²) подход для маленьких наборов
		const size_t numComponents = m_CollisionComponents.size ();

		for (size_t i = 0; i < numComponents; ++i)
			{
			CBaseCollisionComponent * compA = m_CollisionComponents[ i ];

			if (!IsValidCollisionComponent ( compA ))
				continue;

			for (size_t j = i + 1; j < numComponents; ++j)
				{
				CBaseCollisionComponent * compB = m_CollisionComponents[ j ];

				if (!IsValidCollisionComponent ( compB ))
					continue;

				// Быстрая проверка каналов перед детальной коллизией
				if (!compA->CanCollideWith ( compB ) || !compB->CanCollideWith ( compA ))
					continue;
				FCollisionInfo info {};
				info.ComponentA = compA;
				info.ComponentB = compB;

				// Проверяем коллизию через компонент
				if (compA->CheckCollision ( compB, info ))
					{
					ProcessComponentPair ( info );
					}
				}
			}
		}
	}

	// ============================================================================
	// Capsule Collision Checks
	// ============================================================================

bool CCollisionSystem::CheckSphereCapsule ( CBaseCollisionComponent * sphere,
											CBaseCollisionComponent * capsule,
											FCollisionInfo & outInfo ) const
	{
	CCapsuleComponent * cap = dynamic_cast< CCapsuleComponent * >( capsule );
	if (!cap) return false;

	FVector spherePos = sphere->GetWorldLocation ();
	float sphereRadius = sphere->GetCollisionRadius ();

	// Получаем центры полусфер капсулы
	FVector topCenter = cap->GetTopSphereCenter ();
	FVector bottomCenter = cap->GetBottomSphereCenter ();
	float capRadius = cap->GetRadius ();

	// Находим ближайшую точку на оси капсулы к центру сферы
	FVector axis = topCenter - bottomCenter;
	float axisLength = axis.Length ();

	if (axisLength < 0.001f)
		{
			// Капсула вырождена в сферу
		return CheckSphereSphere ( sphere, capsule, outInfo );
		}

	FVector axisDir = axis / axisLength;
	FVector sphereToBottom = spherePos - bottomCenter;

	// Проекция центра сферы на ось капсулы
	float t = sphereToBottom.Dot ( axisDir );
	t = std::max ( 0.0f, std::min ( t, axisLength ) );

	FVector closestPointOnAxis = bottomCenter + axisDir * t;

	// Проверяем расстояние
	FVector delta = spherePos - closestPointOnAxis;
	float distance = delta.Length ();
	float radiusSum = sphereRadius + capRadius;

	if (distance <= radiusSum)
		{
		outInfo.ComponentA = sphere;
		outInfo.ComponentB = capsule;
		outInfo.Depth = radiusSum - distance;

		if (distance > 0.001f)
			{
			outInfo.Normal = delta / distance;
			outInfo.Location = closestPointOnAxis + outInfo.Normal * capRadius;
			}
		else
			{
			outInfo.Normal = FVector ( 0.0f, 0.0f, 1.0f );
			outInfo.Location = spherePos;
			}

		return true;
		}

	return false;
	}

bool CCollisionSystem::CheckBoxCapsule ( CBaseCollisionComponent * box,
										 CBaseCollisionComponent * capsule,
										 FCollisionInfo & outInfo ) const
	{
	CBoxComponent * boxComp = dynamic_cast< CBoxComponent * >( box );
	CCapsuleComponent * cap = dynamic_cast< CCapsuleComponent * >( capsule );

	if (!boxComp || !cap) return false;

	FVector boxPos = box->GetWorldLocation ();
	FVector boxHalf = boxComp->GetHalfExtents ();
	FQuat boxRot = box->GetOwnerActor ()->GetActorRotationQuat ();

	// Получаем центры полусфер капсулы
	FVector capTop = cap->GetTopSphereCenter ();
	FVector capBottom = cap->GetBottomSphereCenter ();
	float capRadius = cap->GetRadius ();

	// Преобразуем точки капсулы в локальное пространство бокса
	FVector localTop = boxRot.Inverse () * ( capTop - boxPos );
	FVector localBottom = boxRot.Inverse () * ( capBottom - boxPos );
	FVector localAxis = localTop - localBottom;
	float axisLength = localAxis.Length ();

	if (axisLength < 0.001f)
		{
			// Капсула вырождена в сферу - используем sphere-box
		CSphereComponent * tempSphere = dynamic_cast< CSphereComponent * > ( capsule );
		if (tempSphere)
			return CheckSphereBox ( capsule, box, outInfo );
		return false;
		}

	FVector localDir = localAxis / axisLength;

	// Находим ближайшую точку на отрезке капсулы к боксу
	float t = 0.0f;
	FVector localClosest;

	// Для каждого измерения находим параметр t
	for (int i = 0; i < 3; i++)
		{
		if (std::abs ( localDir[ i ] ) > 0.001f)
			{
			float t1 = ( -boxHalf[ i ] - localBottom[ i ] ) / localDir[ i ];
			float t2 = ( boxHalf[ i ] - localBottom[ i ] ) / localDir[ i ];

			float tMin = std::min ( t1, t2 );
			float tMax = std::max ( t1, t2 );

			t = std::max ( t, tMin );
			t = std::min ( t, tMax );
			}
		}

	t = std::max ( 0.0f, std::min ( t, axisLength ) );
	localClosest = localBottom + localDir * t;

	// Ограничиваем точку границами бокса
	localClosest.x = std::max ( -boxHalf.x, std::min ( localClosest.x, boxHalf.x ) );
	localClosest.y = std::max ( -boxHalf.y, std::min ( localClosest.y, boxHalf.y ) );
	localClosest.z = std::max ( -boxHalf.z, std::min ( localClosest.z, boxHalf.z ) );

	// Преобразуем обратно в мировое пространство
	FVector closestPointOnBox = boxPos + boxRot * localClosest;

	// Находим ближайшую точку на оси капсулы
	FVector capAxis = capTop - capBottom;
	float capLength = capAxis.Length ();
	FVector capDir = capAxis / capLength;
	FVector boxToCapBottom = closestPointOnBox - capBottom;
	float capT = boxToCapBottom.Dot ( capDir );
	capT = std::max ( 0.0f, std::min ( capT, capLength ) );
	FVector closestPointOnCapsule = capBottom + capDir * capT;

	// Проверяем расстояние
	FVector delta = closestPointOnBox - closestPointOnCapsule;
	float distance = delta.Length ();

	if (distance <= capRadius)
		{
		outInfo.ComponentA = box;
		outInfo.ComponentB = capsule;
		outInfo.Depth = capRadius - distance;

		if (distance > 0.001f)
			{
			outInfo.Normal = delta / distance;
			outInfo.Location = closestPointOnCapsule + outInfo.Normal * capRadius;
			}
		else
			{
			outInfo.Normal = FVector ( 1.0f, 0.0f, 0.0f );
			outInfo.Location = closestPointOnBox;
			}

		return true;
		}

	return false;
	}

bool CCollisionSystem::CheckCapsuleCapsule ( CBaseCollisionComponent * capA,
											 CBaseCollisionComponent * capB,
											 FCollisionInfo & outInfo ) const
	{
	CCapsuleComponent * capsuleA = dynamic_cast< CCapsuleComponent * >( capA );
	CCapsuleComponent * capsuleB = dynamic_cast< CCapsuleComponent * >( capB );

	if (!capsuleA || !capsuleB) return false;

	// Получаем центры полусфер для обеих капсул
	FVector aTop = capsuleA->GetTopSphereCenter ();
	FVector aBottom = capsuleA->GetBottomSphereCenter ();
	FVector bTop = capsuleB->GetTopSphereCenter ();
	FVector bBottom = capsuleB->GetBottomSphereCenter ();

	float aRadius = capsuleA->GetRadius ();
	float bRadius = capsuleB->GetRadius ();

	// Находим ближайшие точки между отрезками (осями капсул)
	FVector aAxis = aTop - aBottom;
	FVector bAxis = bTop - bBottom;

	float aLen = aAxis.Length ();
	float bLen = bAxis.Length ();

	if (aLen < 0.001f || bLen < 0.001f)
		{
			// Одна из капсул вырождена в сферу
		if (aLen < 0.001f)
			return CheckSphereCapsule ( capA, capB, outInfo );
		else
			return CheckSphereCapsule ( capB, capA, outInfo );
		}

	FVector aDir = aAxis / aLen;
	FVector bDir = bAxis / bLen;

	FVector aStart = aBottom;
	FVector bStart = bBottom;

	// Находим ближайшие точки между двумя отрезками
	FVector delta = bStart - aStart;
	float aDotB = aDir.Dot ( bDir );
	float aDotDelta = aDir.Dot ( delta );
	float bDotDelta = bDir.Dot ( delta );

	float tA, tB;
	float denom = 1.0f - aDotB * aDotB;

	if (std::abs ( denom ) < 0.001f)
		{
			// Отрезки параллельны
		tA = 0.0f;
		tB = bDotDelta;
		}
	else
		{
		tA = ( aDotDelta - aDotB * bDotDelta ) / denom;
		tB = ( aDotB * aDotDelta - bDotDelta ) / denom;
		}

		// Ограничиваем параметры длинами отрезков
	tA = std::max ( 0.0f, std::min ( tA, aLen ) );
	tB = std::max ( 0.0f, std::min ( tB, bLen ) );

	// Ближайшие точки
	FVector closestA = aStart + aDir * tA;
	FVector closestB = bStart + bDir * tB;

	// Проверяем расстояние
	FVector deltaAB = closestB - closestA;
	float distance = deltaAB.Length ();
	float radiusSum = aRadius + bRadius;

	if (distance <= radiusSum)
		{
		outInfo.ComponentA = capA;
		outInfo.ComponentB = capB;
		outInfo.Depth = radiusSum - distance;

		if (distance > 0.001f)
			{
			outInfo.Normal = deltaAB / distance;
			outInfo.Location = closestA + outInfo.Normal * aRadius;
			}
		else
			{
			outInfo.Normal = FVector ( 1.0f, 0.0f, 0.0f );
			outInfo.Location = closestA;
			}

		return true;
		}

	return false;
	}

	// ============================================================================
	// Sphere Collision Checks
	// ============================================================================


void CCollisionSystem::ProcessComponentPair ( const FCollisionInfo & collisionInfo )
	{
	CBaseCollisionComponent * compA = collisionInfo.ComponentA;
	CBaseCollisionComponent * compB = collisionInfo.ComponentB;

	if (!compA || !compB)
		return;

	auto pair = std::make_pair ( compA, compB );
	bool bWasColliding = ( m_PreviousFrameCollisions.find ( pair ) != m_PreviousFrameCollisions.end () );

	m_LastFrameCollisions++;
	m_CurrentFrameCollisions.insert ( pair );

	bool bShouldBlock = compA->ShouldBlockWith ( compB ) || compB->ShouldBlockWith ( compA );
	bool bShouldOverlap = compA->ShouldOverlapWith ( compB ) && compB->ShouldOverlapWith ( compA );

	// Разрешаем коллизию если блокирующая
	if (bShouldBlock)
		{
		ResolveCollision ( collisionInfo );
		}

	if (bShouldOverlap && !bWasColliding)
		{
		LOG_DEBUG ( "[COLLISION] New overlap: ", compA->GetName (), " with ", compB->GetName () );

		compA->OnBeginOverlap ( compB );
		compB->OnBeginOverlap ( compA );

		FireCollisionEvent ( ECollisionEventType::BEGIN_OVERLAP, collisionInfo );
		}
	else if (bShouldBlock && !bWasColliding)
		{
		LOG_DEBUG ( "[COLLISION] New hit: ", compA->GetName (), " with ", compB->GetName () );

		compA->OnHit ( compB );
		compB->OnHit ( compA );

		FireCollisionEvent ( ECollisionEventType::COLLISION_HIT, collisionInfo );
		}
	}


bool CCollisionSystem::CheckSphereSphere ( CBaseCollisionComponent * a,
										   CBaseCollisionComponent * b,
										   FCollisionInfo & outInfo ) const
	{
	if (!a || !b) return false;

	FVector posA = a->GetWorldLocation ();
	FVector posB = b->GetWorldLocation ();

	float radiusA = a->GetCollisionRadius ();
	float radiusB = b->GetCollisionRadius ();

	// Try to get exact radius from sphere component
	if (auto * sphereA = dynamic_cast< CSphereComponent * >( a ))
		radiusA = sphereA->GetRadius ();
	if (auto * sphereB = dynamic_cast< CSphereComponent * >( b ))
		radiusB = sphereB->GetRadius ();

	FVector delta = posB - posA;
	float distSq = delta.Dot ( delta );
	float radiusSum = radiusA + radiusB;

	if (distSq <= radiusSum * radiusSum)
		{
		float distance = std::sqrt ( distSq );
		outInfo.Depth = radiusSum - distance;

		if (distance > 0.001f)
			{
			outInfo.Normal = delta / distance;
			outInfo.Location = posA + outInfo.Normal * ( radiusA - outInfo.Depth * 0.5f );
			}
		else
			{
			outInfo.Normal = FVector ( 0.0f, 0.0f, 1.0f );
			outInfo.Location = posA;
			}

		return true;
		}

	return false;
	}

	// ============================================================================
	// Sphere-Box Collision Check
	// ============================================================================

bool CCollisionSystem::CheckSphereBox ( CBaseCollisionComponent * sphere,
										CBaseCollisionComponent * box,
										FCollisionInfo & outInfo ) const
	{
	if (!sphere || !box) return false;

	FVector spherePos = sphere->GetWorldLocation ();
	FVector boxPos = box->GetWorldLocation ();
	float sphereRadius = sphere->GetCollisionRadius ();

	// Get box half extents
	FVector boxHalfExtents;
	if (auto * boxComp = dynamic_cast< CBoxComponent * >( box ))
		{
		boxHalfExtents = boxComp->GetHalfExtents ();
		}
	else
		{
		FVector boundingBox = box->GetBoundingBox ();
		boxHalfExtents = boundingBox * 0.5f;
		}

		// Transform sphere position to box local space
	FQuat boxRot = box->GetOwnerActor ()->GetActorRotationQuat ();
	FVector localSpherePos = boxRot.Inverse () * ( spherePos - boxPos );

	// Find closest point in local space
	FVector localClosest;
	localClosest.x = std::max ( -boxHalfExtents.x, std::min ( localSpherePos.x, boxHalfExtents.x ) );
	localClosest.y = std::max ( -boxHalfExtents.y, std::min ( localSpherePos.y, boxHalfExtents.y ) );
	localClosest.z = std::max ( -boxHalfExtents.z, std::min ( localSpherePos.z, boxHalfExtents.z ) );

	// Transform back to world space
	FVector closestPoint = boxPos + boxRot * localClosest;

	// Check distance
	FVector delta = spherePos - closestPoint;
	float distSq = delta.Dot ( delta );

	if (distSq <= sphereRadius * sphereRadius)
		{
		float distance = std::sqrt ( distSq );
		outInfo.ComponentA = sphere;
		outInfo.ComponentB = box;
		outInfo.Depth = sphereRadius - distance;

		if (distance > 0.001f)
			{
			outInfo.Normal = delta / distance;
			outInfo.Location = closestPoint;
			}
		else
			{
				// Find closest face normal
			float minDist = sphereRadius;
			FVector normal ( 1.0f, 0.0f, 0.0f );

			float distToXMin = std::abs ( localSpherePos.x + boxHalfExtents.x );
			if (distToXMin < minDist) { minDist = distToXMin; normal = FVector ( -1.0f, 0.0f, 0.0f ); }

			float distToXMax = std::abs ( boxHalfExtents.x - localSpherePos.x );
			if (distToXMax < minDist) { minDist = distToXMax; normal = FVector ( 1.0f, 0.0f, 0.0f ); }

			float distToYMin = std::abs ( localSpherePos.y + boxHalfExtents.y );
			if (distToYMin < minDist) { minDist = distToYMin; normal = FVector ( 0.0f, -1.0f, 0.0f ); }

			float distToYMax = std::abs ( boxHalfExtents.y - localSpherePos.y );
			if (distToYMax < minDist) { minDist = distToYMax; normal = FVector ( 0.0f, 1.0f, 0.0f ); }

			float distToZMin = std::abs ( localSpherePos.z + boxHalfExtents.z );
			if (distToZMin < minDist) { minDist = distToZMin; normal = FVector ( 0.0f, 0.0f, -1.0f ); }

			float distToZMax = std::abs ( boxHalfExtents.z - localSpherePos.z );
			if (distToZMax < minDist) { normal = FVector ( 0.0f, 0.0f, 1.0f ); }

			outInfo.Normal = boxRot * normal;
			outInfo.Location = spherePos;
			}

		return true;
		}

	return false;
	}

	// ============================================================================
	// Box-Box Collision Checks
	// ============================================================================

bool CCollisionSystem::CheckAABBAABB ( const FVector & posA, const FVector & halfA,
									   const FVector & posB, const FVector & halfB,
									   FCollisionInfo & outInfo,
									   CBaseCollisionComponent * compA,
									   CBaseCollisionComponent * compB ) const
	{
	FVector delta = posB - posA;

	float overlapX = halfA.x + halfB.x - std::abs ( delta.x );
	float overlapY = halfA.y + halfB.y - std::abs ( delta.y );
	float overlapZ = halfA.z + halfB.z - std::abs ( delta.z );

	if (overlapX > 0 && overlapY > 0 && overlapZ > 0)
		{
			// Find smallest overlap
		float minOverlap = overlapX;
		FVector normal ( 1.0f, 0.0f, 0.0f );

		if (overlapY < minOverlap)
			{
			minOverlap = overlapY;
			normal = FVector ( 0.0f, 1.0f, 0.0f );
			}
		if (overlapZ < minOverlap)
			{
			minOverlap = overlapZ;
			normal = FVector ( 0.0f, 0.0f, 1.0f );
			}

			// Set normal direction
		if (delta.x > 0 && normal.x != 0) normal.x *= -1;
		if (delta.y > 0 && normal.y != 0) normal.y *= -1;
		if (delta.z > 0 && normal.z != 0) normal.z *= -1;

		outInfo.ComponentA = compA;
		outInfo.ComponentB = compB;
		outInfo.Depth = minOverlap;
		outInfo.Normal = normal;
		outInfo.Location = posA + normal * ( halfA.x - minOverlap * 0.5f );

		return true;
		}

	return false;
	}

bool CCollisionSystem::CheckBoxBox ( CBaseCollisionComponent * a,
									 CBaseCollisionComponent * b,
									 FCollisionInfo & outInfo ) const
	{
	if (!a || !b) return false;

	CBoxComponent * boxA = dynamic_cast< CBoxComponent * >( a );
	CBoxComponent * boxB = dynamic_cast< CBoxComponent * >( b );

	if (!boxA || !boxB) return false;

	FVector posA = a->GetWorldLocation ();
	FVector posB = b->GetWorldLocation ();
	FVector halfA = boxA->GetHalfExtents ();
	FVector halfB = boxB->GetHalfExtents ();

	FQuat rotA = a->GetOwnerActor ()->GetActorRotationQuat ();
	FQuat rotB = b->GetOwnerActor ()->GetActorRotationQuat ();

	// If both boxes are axis-aligned, use fast AABB check
	if (rotA.IsIdentity () && rotB.IsIdentity ())
		{
		return CheckAABBAABB ( posA, halfA, posB, halfB, outInfo, a, b );
		}

		// Otherwise use full OBB check
	return CheckOBBOBB ( boxA, boxB, outInfo );
	}

bool CCollisionSystem::CheckOBBOBB ( CBoxComponent * boxA, CBoxComponent * boxB, FCollisionInfo & outInfo ) const
	{
	FVector posA = boxA->GetWorldLocation ();
	FVector posB = boxB->GetWorldLocation ();
	FVector halfA = boxA->GetHalfExtents ();
	FVector halfB = boxB->GetHalfExtents ();

	FQuat rotA = boxA->GetOwnerActor ()->GetActorRotationQuat ();
	FQuat rotB = boxB->GetOwnerActor ()->GetActorRotationQuat ();

	// Get axes in world space
	FVector axesA[ 3 ] = {
		rotA * FVector ( 1.0f, 0.0f, 0.0f ),
		rotA * FVector ( 0.0f, 1.0f, 0.0f ),
		rotA * FVector ( 0.0f, 0.0f, 1.0f )
		};

	FVector axesB[ 3 ] = {
		rotB * FVector ( 1.0f, 0.0f, 0.0f ),
		rotB * FVector ( 0.0f, 1.0f, 0.0f ),
		rotB * FVector ( 0.0f, 0.0f, 1.0f )
		};

		// All 15 axes to test (3 from A, 3 from B, 9 cross products)
	std::vector<FVector> testAxes;

	// Add axes from A and B
	for (int i = 0; i < 3; i++)
		{
		testAxes.push_back ( axesA[ i ] );
		testAxes.push_back ( axesB[ i ] );
		}

		// Add cross products
	for (int i = 0; i < 3; i++)
		{
		for (int j = 0; j < 3; j++)
			{
			FVector cross = axesA[ i ].Cross ( axesB[ j ] );
			if (!cross.IsZero ())
				{
				testAxes.push_back ( cross.Normalized () );
				}
			}
		}

	FVector delta = posB - posA;
	float minOverlap = std::numeric_limits<float>::max ();
	FVector minAxis;

	// Test each axis
	for (const FVector & axis : testAxes)
		{
		if (axis.IsZero ()) continue;

		// Project box A onto axis
		float projA = std::abs ( halfA.x * std::abs ( axis.Dot ( axesA[ 0 ] ) ) ) +
			std::abs ( halfA.y * std::abs ( axis.Dot ( axesA[ 1 ] ) ) ) +
			std::abs ( halfA.z * std::abs ( axis.Dot ( axesA[ 2 ] ) ) );

// Project box B onto axis
		float projB = std::abs ( halfB.x * std::abs ( axis.Dot ( axesB[ 0 ] ) ) ) +
			std::abs ( halfB.y * std::abs ( axis.Dot ( axesB[ 1 ] ) ) ) +
			std::abs ( halfB.z * std::abs ( axis.Dot ( axesB[ 2 ] ) ) );

		float centerDist = std::abs ( delta.Dot ( axis ) );

		if (centerDist > projA + projB)
			{
				// Separating axis found - no collision
			return false;
			}

		float overlap = ( projA + projB ) - centerDist;
		if (overlap < minOverlap)
			{
			minOverlap = overlap;
			minAxis = axis;
			}
		}

		// Collision detected
	outInfo.ComponentA = boxA;
	outInfo.ComponentB = boxB;
	outInfo.Depth = minOverlap;
	outInfo.Normal = minAxis * ( delta.Dot ( minAxis ) > 0 ? -1.0f : 1.0f );

	// Calculate contact point (simplified - midpoint along normal)
	outInfo.Location = posA + outInfo.Normal * ( halfA.Length () - minOverlap * 0.5f );

	return true;
	}

	// ============================================================================
	// Collision Resolution
	// ============================================================================
void CCollisionSystem::ResolveCollision ( const FCollisionInfo & collision )
	{
	if (!collision.ComponentA || !collision.ComponentB)
		return;

	CActor * actorA = collision.ComponentA->GetOwnerActor ();
	CActor * actorB = collision.ComponentB->GetOwnerActor ();

	if (!actorA || !actorB)
		return;

	bool bIsAStatic = ( collision.ComponentA->GetCollisionChannel ().GetName () == "WorldStatic" );
	bool bIsBStatic = ( collision.ComponentB->GetCollisionChannel ().GetName () == "WorldStatic" );

	FVector push = collision.Normal * collision.Depth;

	// Только корректируем позицию, НЕ обнуляем скорость!
	if (bIsAStatic && !bIsBStatic)
		{
		actorB->SetActorLocation ( actorB->GetActorLocation () + push );
		// НЕ вызываем обнуление скорости здесь!
		}
	else if (!bIsAStatic && bIsBStatic)
		{
		actorA->SetActorLocation ( actorA->GetActorLocation () - push );
		// НЕ вызываем обнуление скорости здесь!
		}
	else
		{
		FVector halfPush = push * 0.5f;
		actorA->SetActorLocation ( actorA->GetActorLocation () - halfPush );
		actorB->SetActorLocation ( actorB->GetActorLocation () + halfPush );
		}
	}
	// ============================================================================
	// Event System
	// ============================================================================

void CCollisionSystem::FireCollisionEvent ( ECollisionEventType eventType, const FCollisionInfo & info )
	{
	auto it = m_Callbacks.find ( eventType );
	if (it != m_Callbacks.end ())
		{
		it->second ( info );
		}
	}

void CCollisionSystem::ProcessCollisionsSpatial ()
	{
		// Обновляем пространственное разделение если нужно
	static float gridUpdateTimer = 0.0f;
	gridUpdateTimer += m_AccumulatedTime;

	if (gridUpdateTimer >= 0.1f) // Обновляем сетку 10 раз в секунду
		{
		UpdateSpatialPartition ();
		gridUpdateTimer = 0.0f;
		}

		// Множество для отслеживания обработанных пар
	std::set<std::pair<CBaseCollisionComponent *, CBaseCollisionComponent *>> processedPairs;

	for (CBaseCollisionComponent * compA : m_CollisionComponents)
		{
		if (!IsValidCollisionComponent ( compA ))
			continue;

		// Получаем потенциально сталкивающиеся компоненты
		auto potentialCollisions = GetPotentiallyCollidingComponents ( compA );

		for (CBaseCollisionComponent * compB : potentialCollisions)
			{
			if (!IsValidCollisionComponent ( compB ))
				continue;

			// Убеждаемся что обрабатываем каждую пару только один раз
			auto pair = std::make_pair ( compA, compB );
			auto reversePair = std::make_pair ( compB, compA );

			if (processedPairs.find ( pair ) != processedPairs.end () ||
				 processedPairs.find ( reversePair ) != processedPairs.end ())
				{
				continue;
				}

				// Быстрая проверка каналов
			if (!compA->CanCollideWith ( compB ) || !compB->CanCollideWith ( compA ))
				continue;
			FCollisionInfo Info {};
			Info.ComponentA = compA;
			Info.ComponentB = compB;
			// Проверяем коллизию
			if (compA->CheckCollision ( compB, Info ))
				{
				processedPairs.insert ( pair );
				ProcessComponentPair ( Info );
				}
			}
		}
	}

void CCollisionSystem::RegisterCollisionCallback ( ECollisionEventType eventType,
												   const FCollisionCallback & callback )
	{
	m_Callbacks[ eventType ] = callback;
	}

void CCollisionSystem::UnregisterCollisionCallback ( ECollisionEventType eventType )
	{
	m_Callbacks.erase ( eventType );
	}

	// ============================================================================
	// Raycasting
	// ============================================================================

FRaycastResult CCollisionSystem::Raycast ( const FVector & start,
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

FRaycastResult CCollisionSystem::Raycast ( const FVector & start,
										   const FVector & direction,
										   float distance,
										   const std::string & channelName ) const
	{
	FRaycastResult result;
	float closestDistance = std::numeric_limits<float>::max ();

	for (CBaseCollisionComponent * component : m_CollisionComponents)
		{
		if (!component || !component->IsCollisionEnabled ())
			continue;

		if (channelName != "All" && !component->CanCollideWith ( channelName ))
			continue;

		if (!component->GetOwnerActor ())
			continue;

		// Для террейна используем специальную проверку
		if (component->GetShapeType () == ECollisionShape::TERRAIN)
			{
			FVector hitPoint, normal;
			float hitDist;

			if (CheckRayTerrain ( start, direction, distance, component, hitPoint, normal, hitDist ))
				{
				if (hitDist < closestDistance)
					{
					closestDistance = hitDist;
					result.bHit = true;
					result.HitComponent = component;
					result.Location = hitPoint;
					result.Normal = normal;
					result.Distance = hitDist;
					}
				}
			continue;
			}

			// Для остальных компонентов - существующая проверка
		FVector compPos = component->GetOwnerActor ()->GetActorLocation ();
		float radius = component->GetCollisionRadius ();

		FVector toSphere = compPos - start;
		float projection = toSphere.Dot ( direction );

		if (projection < 0.0f || projection > distance)
			continue;

		FVector closestPoint = start + direction * projection;
		FVector toClosest = closestPoint - compPos;
		float distSq = toClosest.Dot ( toClosest );

		if (distSq <= radius * radius)
			{
			float hitDistance = projection - std::sqrt ( radius * radius - distSq );

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

	// ============================================================================
	// Overlap Tests
	// ============================================================================

std::vector<CBaseCollisionComponent *> CCollisionSystem::SphereOverlap (
	const FVector & center, float radius, const std::string & channelName ) const
	{
	std::vector<CBaseCollisionComponent *> results;

	for (CBaseCollisionComponent * component : m_CollisionComponents)
		{
		if (!component || !component->IsCollisionEnabled ())
			continue;

		if (channelName != "All" && !component->CanCollideWith ( channelName ))
			continue;

		if (!component->GetOwnerActor ())
			continue;

		FVector compPos = component->GetOwnerActor ()->GetActorLocation ();
		float compRadius = component->GetCollisionRadius ();

		FVector delta = compPos - center;
		float distSq = delta.Dot ( delta );
		float radiusSum = radius + compRadius;

		if (distSq <= radiusSum * radiusSum)
			{
			results.push_back ( component );
			}
		}

	return results;
	}

std::vector<CBaseCollisionComponent *> CCollisionSystem::BoxOverlap (
	const FVector & center, const FVector & halfExtents, const FVector & rotation,
	const std::string & channelName ) const
	{
		// Simple implementation - convert to sphere check
		// TODO: Implement proper Box-Box overlap test
	float sphereRadius = std::max ( halfExtents.x, std::max ( halfExtents.y, halfExtents.z ) );
	return SphereOverlap ( center, sphereRadius, channelName );
	}

	// ============================================================================
	// Manual Collision Checks
	// ============================================================================

std::vector<FCollisionInfo> CCollisionSystem::CheckCollisions (
	CBaseCollisionComponent * component ) const
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
			float distSq = delta.Dot ( delta );
			float totalRadiusSq = totalRadius * totalRadius;

			if (distSq <= totalRadiusSq)
				{
				float distance = std::sqrt ( distSq );

				if (distance > 0.001f)
					{
					info.Normal = delta / distance;
					info.Depth = totalRadius - distance;
					info.Location = posA + info.Normal * ( radiusA - info.Depth * 0.5f );
					}
				else
					{
					info.Normal = FVector ( 0.0f, 0.0f, 1.0f );
					info.Depth = totalRadius;
					info.Location = posA;
					}

				collisions.push_back ( info );
				}
			}
		}

	return collisions;
	}

std::vector<FCollisionInfo> CCollisionSystem::CheckCollisionsAtLocation (
	const FVector & location, float radius ) const
	{
	std::vector<FCollisionInfo> collisions;
	auto components = SphereOverlap ( location, radius, "All" );

	for (CBaseCollisionComponent * comp : components)
		{
		if (!comp || !comp->GetOwnerActor ())
			continue;

		FCollisionInfo info;
		info.ComponentA = nullptr; // Mark as area check
		info.ComponentB = comp;
		info.Location = comp->GetOwnerActor ()->GetActorLocation ();
		info.Normal = ( info.Location - location ).Normalized ();
		info.Depth = radius - ( info.Location - location ).Length ();

		collisions.push_back ( info );
		}

	return collisions;
	}

	// ============================================================================
	// Spatial Partitioning
	// ============================================================================

void CCollisionSystem::UpdateSpatialPartition ()
	{
	m_SpatialGrid.clear ();

	for (CBaseCollisionComponent * component : m_CollisionComponents)
		{
		if (!component || !component->GetOwnerActor ())
			continue;

		FVector position = component->GetOwnerActor ()->GetActorLocation ();

		int cellX = static_cast< int >( position.x / m_CellSize );
		int cellY = static_cast< int >( position.y / m_CellSize );
		int cellZ = static_cast< int >( position.z / m_CellSize );

		int64_t cellKey = ( static_cast< int64_t >( cellX ) << 42 ) |
			( static_cast< int64_t >( cellY ) << 21 ) |
			static_cast< int64_t >( cellZ );

		m_SpatialGrid[ cellKey ].Components.push_back ( component );
		}
	}

float CCollisionSystem::GetComponentBoundingRadius ( CBaseCollisionComponent * component ) const
	{
	if (!component) return 0.0f;

	switch (component->GetShapeType ())
		{
			case ECollisionShape::SPHERE:
				return component->GetCollisionRadius ();

			case ECollisionShape::BOX:
				if (CBoxComponent * box = dynamic_cast< CBoxComponent * >( component ))
					{
					FVector half = box->GetHalfExtents ();
					return half.Length ();
					}
				break;

			case ECollisionShape::CAPSULE:
				if (CCapsuleComponent * cap = dynamic_cast< CCapsuleComponent * >( component ))
					{
					return cap->GetRadius () + cap->GetHalfHeight ();
					}
				break;

			case ECollisionShape::TERRAIN:
				if (CTerrainComponent * terr = dynamic_cast< CTerrainComponent * >( component ))
					{
					FVector box = terr->GetBoundingBox ();
					return box.Length () * 0.5f; // Половина диагонали
					}
				break;

			default:
				break;
		}

	return 100.0f; // Значение по умолчанию
	}


std::vector<CBaseCollisionComponent *> CCollisionSystem::GetPotentiallyCollidingComponents (
	CBaseCollisionComponent * component ) const
	{
	std::vector<CBaseCollisionComponent *> result;

	if (!component || !component->GetOwnerActor ())
		return result;

	FVector position = component->GetOwnerActor ()->GetActorLocation ();
	float radius = GetComponentBoundingRadius ( component );

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
								// Дополнительная проверка расстояния
							FVector otherPos = other->GetOwnerActor ()->GetActorLocation ();
							float distSq = ( otherPos - position ).LengthSquared ();
							float maxDist = radius + GetComponentBoundingRadius ( other );

							if (distSq <= maxDist * maxDist)
								{
								result.push_back ( other );
								}
							}
						}
					}
				}
			}
		}

	return result;
	}

// ============================================================================
// Terrain Collision Checks
// ============================================================================

bool CCollisionSystem::CheckSphereTerrain ( CBaseCollisionComponent * sphere,
											CBaseCollisionComponent * terrain,
											FCollisionInfo & outInfo ) const
	{
	CTerrainComponent * terr = dynamic_cast< CTerrainComponent * >( terrain );
	if (!terr) return false;

	FVector spherePos = sphere->GetWorldLocation ();
	float sphereRadius = sphere->GetCollisionRadius ();

	// Получаем высоту террейна под центром сферы
	float terrainHeight = terr->GetHeightAtWorld ( spherePos );

	// Проверяем, касается ли сфера террейна
	if (spherePos.y - sphereRadius <= terrainHeight)
		{
		outInfo.ComponentA = sphere;
		outInfo.ComponentB = terrain;
		outInfo.Depth = ( terrainHeight - ( spherePos.y - sphereRadius ) );
		outInfo.Normal = FVector ( 0.0f, -1.0f, 0.0f ); // Нормаль вверх
		outInfo.Location = FVector ( spherePos.x, terrainHeight, spherePos.z );

		return true;
		}

	return false;
	}


bool CCollisionSystem::CheckBoxTerrain ( CBaseCollisionComponent * box,
										 CBaseCollisionComponent * terrain,
										 FCollisionInfo & outInfo ) const
	{
	CBoxComponent * boxComp = dynamic_cast< CBoxComponent * >( box );
	CTerrainComponent * terr = dynamic_cast< CTerrainComponent * >( terrain );

	if (!boxComp || !terr) return false;

	FVector boxPos = box->GetWorldLocation ();
	FVector half = boxComp->GetHalfExtents ();

	// Находим самую нижнюю точку бокса (с учётом вращения)
	FQuat boxRot = box->GetOwnerActor ()->GetActorRotationQuat ();

	// 8 углов бокса
	FVector corners[ 8 ] = {
		boxRot * FVector ( -half.x, -half.y, -half.z ) + boxPos,
		boxRot * FVector ( half.x, -half.y, -half.z ) + boxPos,
		boxRot * FVector ( -half.x,  half.y, -half.z ) + boxPos,
		boxRot * FVector ( half.x,  half.y, -half.z ) + boxPos,
		boxRot * FVector ( -half.x, -half.y,  half.z ) + boxPos,
		boxRot * FVector ( half.x, -half.y,  half.z ) + boxPos,
		boxRot * FVector ( -half.x,  half.y,  half.z ) + boxPos,
		boxRot * FVector ( half.x,  half.y,  half.z ) + boxPos
		};

		// Находим самую нижнюю точку
	float minY = corners[ 0 ].y;
	for (int i = 1; i < 8; i++)
		{
		if (corners[ i ].y < minY)
			minY = corners[ i ].y;
		}

		// Получаем высоту террейна под центром бокса
	float terrainHeight = terr->GetHeightAtWorld ( boxPos );

	// Проверяем, касается ли бокс террейна
	if (minY <= terrainHeight)
		{
		outInfo.ComponentA = box;
		outInfo.ComponentB = terrain;
		outInfo.Depth = terrainHeight - minY;
		outInfo.Normal = FVector ( 0.0f, -1.0f, 0.0f ); // Та же нормаль, что и у сферы
		outInfo.Location = FVector ( boxPos.x, terrainHeight, boxPos.z );

		return true;
		}

	return false;
	}

bool CCollisionSystem::CheckCapsuleTerrain ( CBaseCollisionComponent * capsule,
											 CBaseCollisionComponent * terrain,
											 FCollisionInfo & outInfo ) const
	{
	CCapsuleComponent * cap = dynamic_cast< CCapsuleComponent * >( capsule );
	CTerrainComponent * terr = dynamic_cast< CTerrainComponent * >( terrain );

	if (!cap || !terr) return false;

	// Получаем центры полусфер капсулы
	FVector topCenter = cap->GetTopSphereCenter ();
	FVector bottomCenter = cap->GetBottomSphereCenter ();
	float radius = cap->GetRadius ();

	// Проверяем нижнюю полусферу (самая нижняя точка)
	float lowestPoint = bottomCenter.y - radius;

	// Получаем высоту террейна под центром капсулы
	FVector capsulePos = capsule->GetWorldLocation ();
	float terrainHeight = terr->GetHeightAtWorld ( capsulePos );

	// Проверяем, касается ли капсула террейна
	if (lowestPoint <= terrainHeight)
		{
		outInfo.ComponentA = capsule;
		outInfo.ComponentB = terrain;
		outInfo.Depth = terrainHeight - lowestPoint;
		outInfo.Normal = FVector ( 0.0f, -1.0f, 0.0f ); // Та же нормаль, что и у сферы
		outInfo.Location = FVector ( capsulePos.x, terrainHeight, capsulePos.z );

		return true;
		}

	return false;
	}

bool CCollisionSystem::CheckRayTerrain ( const FVector & start, const FVector & direction, float maxDistance,
										 CBaseCollisionComponent * terrain,
										 FVector & outHit, FVector & outNormal, float & outDist ) const
	{
	CTerrainComponent * terr = dynamic_cast< CTerrainComponent * >( terrain );
	if (!terr) return false;

	// Простой DDA (Digital Differential Analyzer) алгоритм для рейкаста по террейну
	float step = terr->GetTerrainData ().CellSize * 0.5f;
	FVector current = start;
	float traveled = 0.0f;

	while (traveled < maxDistance)
		{
		float terrainY = terr->GetHeightAtWorld ( current );

		if (current.y <= terrainY)
			{
			outHit = current;
			outNormal = FVector ( 0.0f, 1.0f, 0.0f );
			outDist = traveled;
			return true;
			}

		current += direction * step;
		traveled += step;
		}

	return false;
	}