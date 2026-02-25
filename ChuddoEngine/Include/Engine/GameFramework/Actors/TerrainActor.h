#pragma once

#include "Actor.h"
#include "Render/RenderInfo.h"
class CTerrainComponent;
class CTerrainMeshComponent;  // Добавляем

class CTerrainActor : public CActor
    {
    CHUDDO_DECLARE_CLASS ( CTerrainActor, CActor );

    public:
        CTerrainActor ( CObject * inOwner = nullptr, const std::string & inDisplayName = "TerrainActor" );
        virtual ~CTerrainActor ();

        virtual void BeginPlay () override;
        virtual void Tick ( float deltaTime ) override;
        virtual void EndPlay () override;

        // Переопределяем метод сбора мешей
        virtual std::vector<FMeshInfo> GetRenderMeshes () const override;

        // Доступ к компонентам
        CTerrainComponent * GetTerrainComponent () const { return m_TerrainComponent; }
        CTerrainMeshComponent * GetTerrainMeshComponent () const;  // Добавляем

        // Удобные методы для создания террейна
        void GenerateFlat ( int32 width, int32 height, float cellSize, float heightValue = 0.0f );
        void GenerateFromHeightmap ( const std::vector<float> & heights, int32 width, int32 height, float cellSize );

        void GenerateHilly ( int32 width, int32 height, float cellSize,
                             float amplitude = 50.0f, float frequency = 0.05f );
        void GenerateNoise ( int32 width, int32 height, float cellSize, int32 seed = 0 );
        void GenerateCustom ( int32 width, int32 height, float cellSize,
                              std::function<float ( int32 x, int32 z )> heightFunc );


    private:
        CTerrainComponent * m_TerrainComponent = nullptr;
        CTerrainMeshComponent * m_TerrainMeshComponent = nullptr;  // Добавляем
    };

REGISTER_CLASS_FACTORY ( CTerrainActor );