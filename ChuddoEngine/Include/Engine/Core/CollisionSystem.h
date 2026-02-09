#pragma once
#include "CoreMinimal.h"
#include "Components/BaseCollisionComponent.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

// Типы событий коллизий
enum class ECollisionEventType : uint8_t
    {
    BEGIN_OVERLAP,
    END_OVERLAP,
    COLLISION_HIT
    };

    // Структура для хранения информации о столкновении
struct FCollisionInfo
    {
    CBaseCollisionComponent * ComponentA;
    CBaseCollisionComponent * ComponentB;
    FVector Location; // Точка столкновения
    FVector Normal;   // Нормаль столкновения
    float Depth;      // Глубина проникновения
    };

    // Callback для событий коллизий
using FCollisionCallback = std::function<void ( const FCollisionInfo & )>;

// Класс для управления системой коллизий
class CCollisionSystem : public CObject
    {
    CHUDDO_DECLARE_CLASS ( CCollisionSystem, CObject );

    public:
        // Конструктор должен соответствовать сигнатуре CObject
        CCollisionSystem ( CObject * inOwner = nullptr,
                           const std::string & inDisplayName = "CollisionSystem" );
        virtual ~CCollisionSystem ();

        // Удаляем копирование и присваивание
        CCollisionSystem ( const CCollisionSystem & ) = delete;
        CCollisionSystem & operator=( const CCollisionSystem & ) = delete;

        // Статический доступ к системе (синглтон)
        static CCollisionSystem & Get ();

        // Регистрация/удаление коллизионных компонентов
        void RegisterCollisionComponent ( CBaseCollisionComponent * component );
        void UnregisterCollisionComponent ( CBaseCollisionComponent * component );

        // Основное обновление системы коллизий
        void Update ( float deltaTime );

        // Ручная проверка столкновений
        std::vector<FCollisionInfo> CheckCollisions ( CBaseCollisionComponent * component ) const;
        std::vector<FCollisionInfo> CheckCollisionsAtLocation ( const FVector & location, float radius ) const;

        // Raycasting
        struct FRaycastResult
            {
            bool bHit = false;
            CBaseCollisionComponent * HitComponent = nullptr;
            FVector Location;
            FVector Normal;
            float Distance = 0.0f;
            };

        FRaycastResult Raycast ( const FVector & start, const FVector & end,
                                 const std::string & channelName = "All" ) const;
        FRaycastResult Raycast ( const FVector & start, const FVector & direction, float distance,
                                 const std::string & channelName = "All" ) const;

          // Сферическая проверка
        std::vector<CBaseCollisionComponent *> SphereOverlap ( const FVector & center, float radius,
                                                               const std::string & channelName = "All" ) const;

            // Box проверка
        std::vector<CBaseCollisionComponent *> BoxOverlap ( const FVector & center, const FVector & halfExtents,
                                                            const FVector & rotation,
                                                            const std::string & channelName = "All" ) const;

            // Регистрация callback'ов
        void RegisterCollisionCallback ( ECollisionEventType eventType, const FCollisionCallback & callback );
        void UnregisterCollisionCallback ( ECollisionEventType eventType );

        // Отладка и статистика
        void EnableDebugDraw ( bool enable ) { bDebugDraw = enable; }
        bool IsDebugDrawEnabled () const { return bDebugDraw; }

        int32 GetRegisteredComponentsCount () const { return static_cast< int32 >( m_CollisionComponents.size () ); }
        int32 GetActiveCollisionsCount () const { return m_LastFrameCollisions; }

        // Оптимизация
        void SetUpdateRate ( float rateHz ) { m_UpdateRate = rateHz; }
        void EnableSpatialPartition ( bool enable ) { bUseSpatialPartition = enable; }

    private:
        // Внутренние методы
        void ProcessCollisions ();
        void ResolveCollision ( const FCollisionInfo & collision );
        void FireCollisionEvent ( ECollisionEventType eventType, const FCollisionInfo & info );

        // Методы проверки геометрии
        bool CheckSphereSphere ( CBaseCollisionComponent * a, CBaseCollisionComponent * b, FCollisionInfo & outInfo ) const;
        bool CheckSphereBox ( CBaseCollisionComponent * sphere, CBaseCollisionComponent * box, FCollisionInfo & outInfo ) const;
        bool CheckBoxBox ( CBaseCollisionComponent * a, CBaseCollisionComponent * b, FCollisionInfo & outInfo ) const;

        // Пространственное разделение (простая реализация)
        struct FSpatialCell
            {
            std::vector<CBaseCollisionComponent *> Components;
            };

        void UpdateSpatialPartition ();
        std::vector<CBaseCollisionComponent *> GetPotentiallyCollidingComponents ( CBaseCollisionComponent * component ) const;

        // Члены класса
        std::vector<CBaseCollisionComponent *> m_CollisionComponents;
        std::unordered_map<CBaseCollisionComponent *, FVector> m_LastPositions;

        // Пространственное разделение
        bool bUseSpatialPartition = false;
        float m_CellSize = 500.0f;
        std::unordered_map<int64_t, FSpatialCell> m_SpatialGrid;

        // Callback'и
        std::unordered_map<ECollisionEventType, FCollisionCallback> m_Callbacks;

        // Статистика и отладка
        bool bDebugDraw = false;
        int32 m_LastFrameCollisions = 0;
        float m_UpdateRate = 60.0f; // Гц
        float m_AccumulatedTime = 0.0f;

        // Static instance
        static std::unique_ptr<CCollisionSystem> s_Instance;
        static bool s_IsInitialized;
    };

    // Макросы для удобства
#define COLLISION_SYSTEM CCollisionSystem::Get()
    REGISTER_CLASS_FACTORY ( CCollisionSystem );