#pragma once

#include "BaseCollisionComponent.h"
#include <vector>
#include <cstdint>


// Структура для хранения данных ландшафта
struct FTerrainData
    {
    std::vector<float> Heights;      // Высоты в каждой точке
    int32 Width = 0;                  // Количество точек по X
    int32 Height = 0;                  // Количество точек по Z
    float CellSize = 100.0f;           // Размер ячейки в мировых единицах
    FVector Origin = FVector::Zero ();  // Центр ландшафта

    // Минимальная/максимальная высота
    float MinHeight = 0.0f;
    float MaxHeight = 0.0f;

    // Получить высоту в точке (x, z) в локальных координатах
    float GetHeight ( int32 x, int32 z ) const
        {
        if (x < 0 || x >= Width || z < 0 || z >= Height)
            return 0.0f;
        return Heights[ z * Width + x ];
        }

        // Получить интерполированную высоту в произвольной точке
    float GetInterpolatedHeight ( float worldX, float worldZ ) const;
    };

class CTerrainComponent : public CBaseCollisionComponent
    {
    CHUDDO_DECLARE_CLASS ( CTerrainComponent, CBaseCollisionComponent );

    public:
        CTerrainComponent ( CObject * inOwner = nullptr, const std::string & InName = "TerrainComponent" );
        virtual ~CTerrainComponent ();

        virtual void InitComponent () override;
        virtual void Tick ( float DeltaTime ) override;
        virtual void OnBeginPlay () override;

        // Основные методы коллизий
        virtual bool CheckCollision ( CBaseCollisionComponent * other, FCollisionInfo & outInfo ) const override;
        virtual float GetCollisionRadius () const override { return 0.0f; }
        virtual FVector GetBoundingBox () const override;

        // Настройка террейна
        void GenerateFlat ( int32 width, int32 height, float cellSize, float heightValue = 0.0f );
        void GenerateFromHeightmap ( const std::vector<float> & heights, int32 width, int32 height, float cellSize );
        void SetHeightAt ( int32 x, int32 z, float height );

        // Получение высоты в точке
        float GetHeightAtWorld ( const FVector & worldPos ) const;
        float GetHeightAtLocal ( float localX, float localZ ) const;

        // Информация о террейне
        const FTerrainData & GetTerrainData () const { return m_TerrainData; }
        FVector GetWorldPositionAt ( int32 x, int32 z ) const;


        // Отладка
        void EnableDebugDraw ( bool enable ) { m_bDebugDraw = enable; }
       
    private:
        
        // Вспомогательные методы
        bool RaycastTerrain ( const FVector & start, const FVector & dir, float maxDist,
                              FVector & outHit, FVector & outNormal, float & outDist ) const;

        FTerrainData m_TerrainData;
   
        bool m_bDebugDraw = false;
    };

REGISTER_CLASS_FACTORY ( CTerrainComponent );