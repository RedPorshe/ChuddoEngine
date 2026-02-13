#pragma once

#include "Actor.h"
class CTerrainComponent;

class CTerrainActor : public CActor
    {
    CHUDDO_DECLARE_CLASS ( CTerrainActor, CActor );

    public:
        CTerrainActor ( CObject * inOwner = nullptr, const std::string & inDisplayName = "TerrainActor" );
        virtual ~CTerrainActor ();

        virtual void BeginPlay () override;
        virtual void Tick ( float deltaTime ) override;
        virtual void EndPlay () override;

        // Доступ к компоненту террейна
        CTerrainComponent * GetTerrainComponent () const { return m_TerrainComponent; }

        // Удобные методы для создания террейна
        void GenerateFlat ( int32 width, int32 height, float cellSize, float heightValue = 0.0f );
        void GenerateFromHeightmap ( const std::vector<float> & heights, int32 width, int32 height, float cellSize );

    private:
        CTerrainComponent * m_TerrainComponent = nullptr;
    };

REGISTER_CLASS_FACTORY ( CTerrainActor );