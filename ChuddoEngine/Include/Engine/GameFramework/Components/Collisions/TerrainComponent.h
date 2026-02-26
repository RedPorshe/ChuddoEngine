#pragma once

#include "BaseCollisionComponent.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

class CBufferManager;

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

// Структура вершины для рендера террейна
struct FTerrainVertex
    {
    FVector Position;
    FVector Normal;
    FVector Color;
    FVector2D UV;

    static VkVertexInputBindingDescription GetBindingDescription ()
        {
        VkVertexInputBindingDescription binding {};
        binding.binding = 0;
        binding.stride = sizeof ( FTerrainVertex );
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding;
        }

    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions ()
        {
        std::vector<VkVertexInputAttributeDescription> attributes ( 4 );

        // Position
        attributes[ 0 ].binding = 0;
        attributes[ 0 ].location = 0;
        attributes[ 0 ].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[ 0 ].offset = offsetof ( FTerrainVertex, Position );

        // Normal
        attributes[ 1 ].binding = 0;
        attributes[ 1 ].location = 1;
        attributes[ 1 ].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[ 1 ].offset = offsetof ( FTerrainVertex, Normal );

        // Color
        attributes[ 2 ].binding = 0;
        attributes[ 2 ].location = 2;
        attributes[ 2 ].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[ 2 ].offset = offsetof ( FTerrainVertex, Color );

        // UV
        attributes[ 3 ].binding = 0;
        attributes[ 3 ].location = 3;
        attributes[ 3 ].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[ 3 ].offset = offsetof ( FTerrainVertex, UV );

        return attributes;
        }
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

        // ========== КОЛЛИЗИИ ==========
        virtual bool CheckCollision ( CBaseCollisionComponent * other, FCollisionInfo & outInfo ) const override;
        virtual float GetCollisionRadius () const override { return 0.0f; }
        virtual FVector GetBoundingBox () const override;

        // ========== ГЕНЕРАЦИЯ ТЕРРЕЙНА ==========
        void GenerateFlat ( int32 width, int32 height, float cellSize, float heightValue = 0.0f );
        void GenerateFromHeightmap ( const std::vector<float> & heights, int32 width, int32 height, float cellSize );
        void SetHeightAt ( int32 x, int32 z, float height );

        /**
     * Создать холмистый террейн с использованием синусоидальной функции
     * @param width - количество точек по X
     * @param height - количество точек по Z
     * @param cellSize - размер ячейки
     * @param amplitude - амплитуда холмов (максимальная высота)
     * @param frequency - частота холмов (чем больше, тем чаще холмы)
     */
        void GenerateHilly ( int32 width, int32 height, float cellSize,
                             float amplitude = 50.0f, float frequency = 0.05f );

          /**
           * Создать холмистый террейн с использованием шума Перлина (упрощённый)
           * @param width - количество точек по X
           * @param height - количество точек по Z
           * @param cellSize - размер ячейки
           * @param seed - сид для генератора случайных чисел
           */
        void GenerateNoise ( int32 width, int32 height, float cellSize, int32 seed = 0 );

        /**
         * Создать террейн с кастомной функцией высоты
         * @param width - количество точек по X
         * @param height - количество точек по Z
         * @param cellSize - размер ячейки
         * @param heightFunc - функция, принимающая (x, z) и возвращающая высоту
         */
        void GenerateCustom ( int32 width, int32 height, float cellSize,
                              std::function<float ( int32 x, int32 z )> heightFunc );

          /**
           * Загрузить террейн из heightmap изображения (черно-белого)
           * @param filename - путь к файлу изображения
           * @param cellSize - размер ячейки
           */
        bool LoadFromHeightmap ( const std::string & filename, float cellSize );


        // ========== ЗАПРОСЫ ВЫСОТЫ ==========
        float GetHeightAtWorld ( const FVector & worldPos ) const;
        float GetHeightAtLocal ( float localX, float localZ ) const;
        FVector GetWorldPositionAt ( int32 x, int32 z ) const;
       

        // ========== ДАННЫЕ ТЕРРЕЙНА ==========
        const FTerrainData & GetTerrainData () const { return m_TerrainData; }

        // ========== РЕНДЕР РЕСУРСЫ ==========
        void CreateRenderResources ( CBufferManager * BufferManager );
        void DestroyRenderResources ();

        bool HasRenderResources () const
            {
            return m_VertexBuffer != VK_NULL_HANDLE && m_VertexCount > 0;
            }

        VkBuffer GetVertexBuffer () const { return m_VertexBuffer; }
        VkBuffer GetIndexBuffer () const { return m_IndexBuffer; }
        uint32_t GetVertexCount () const { return m_VertexCount; }
        uint32_t GetIndexCount () const { return m_IndexCount; }

        // ========== ОТЛАДКА ==========
        void EnableDebugDraw ( bool enable ) { m_bDebugDraw = enable; }

    private:
        // ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========
        bool RaycastTerrain ( const FVector & start, const FVector & dir, float maxDist,
                              FVector & outHit, FVector & outNormal, float & outDist ) const;

        // ========== ГЕНЕРАЦИЯ ГЕОМЕТРИИ ДЛЯ РЕНДЕРА ==========
        void GenerateVertices ( std::vector<FTerrainVertex> & vertices ) const;
        void GenerateIndices ( std::vector<uint32_t> & indices ) const;

    private:
        // ========== ДАННЫЕ ТЕРРЕЙНА ==========
        FTerrainData m_TerrainData;

        // ========== РЕНДЕР РЕСУРСЫ ==========
        VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
        VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
        uint32_t m_VertexCount = 0;
        uint32_t m_IndexCount = 0;

        // ========== ОТЛАДКА ==========
        bool m_bDebugDraw = false;
    };

REGISTER_CLASS_FACTORY ( CTerrainComponent );