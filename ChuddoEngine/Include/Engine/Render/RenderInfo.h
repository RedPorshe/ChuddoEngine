#pragma once

#include "CoreMinimal.h"
#include "Utils/Math/MathTypes.h"
#include <vulkan/vulkan.h>
#include <vector>

struct FCameraInfo
    {
    // Матрицы камеры
    FMat4 ViewMatrix { 1.f };
    FMat4 ProjectionMatrix { 1.f };

    // Позиционные данные
    FVector Location { FVector::Zero () };
    FVector ViewTarget { FVector::Zero () };

    // Параметры проекции
    float NearPlane { 0.1f };
    float FarPlane { 1000.f };
    float FOV { 90.f }; // в градусах

    // Конструкторы
    FCameraInfo () = default;

    FCameraInfo ( const FVector & inLocation  ,
                  const FVector & inViewTarget ,
                  const FMat4 & inViewMatrix = FMat4 ( 1.f ),
                  const FMat4 & inProjectionMatrix = FMat4 ( 1.f ),
                  const float & inNearPlane = 0.1f,
                  const float & inFarPlane = 1000.f,
                  const float & inFOVAngles = 90.f );

      // Методы
    void Clear ();
    void UpdateViewMatrix ();
    void UpdateProjectionMatrix ( float AspectRatio );

    // Геттеры
    FMat4 GetViewMatrix () const { return ViewMatrix; }
    FMat4 GetProjectionMatrix () const { return ProjectionMatrix; }
    FVector GetLocation () const { return Location; }
    FVector GetViewTarget () const { return ViewTarget; }

    // Сеттеры
    void SetLocation ( const FVector & InLocation ) { Location = InLocation; }
    void SetViewTarget ( const FVector & InViewTarget ) { ViewTarget = InViewTarget; }
    void SetFOV ( float InFOV ) { FOV = InFOV; }
    void SetNearPlane ( float InNearPlane ) { NearPlane = InNearPlane; }
    void SetFarPlane ( float InFarPlane ) { FarPlane = InFarPlane; }
    };

struct FMeshInfo
    {
    // Матрица трансформации меша
    FMat4 Model = FMat4 ( 1.f );

    // Vulkan буферы (для отрисовки)
    VkBuffer VertexBuffer = VK_NULL_HANDLE;
    VkBuffer IndexBuffer = VK_NULL_HANDLE;

    // Количество вершин/индексов
    uint32_t VertexCount = 0;
    uint32_t IndexCount = 0;

    // ID материала (для будущего использования)
    uint32_t MaterialId = 0;

    // Имя пайплайна для рендера
    std::string PipelineName = "StaticMesh";

    // Конструктор
    FMeshInfo () = default;

    FMeshInfo ( VkBuffer InVertexBuffer, uint32_t InVertexCount,
                VkBuffer InIndexBuffer = VK_NULL_HANDLE, uint32_t InIndexCount = 0,
                const FMat4 & InModel = FMat4 ( 1.f ),
                const std::string & InPipelineName = "StaticMesh" )
        : Model ( InModel )
        , VertexBuffer ( InVertexBuffer )
        , IndexBuffer ( InIndexBuffer )
        , VertexCount ( InVertexCount )
        , IndexCount ( InIndexCount )
        , MaterialId ( 0 )
        , PipelineName ( InPipelineName )
        {}

    // Метод очистки
    void Clear ()
        {
        Model = FMat4 ( 1.f );
        VertexBuffer = VK_NULL_HANDLE;
        IndexBuffer = VK_NULL_HANDLE;
        VertexCount = 0;
        IndexCount = 0;
        MaterialId = 0;
        PipelineName.clear ();
        }

    // Проверка валидности
    bool IsValid () const
        {
        return VertexBuffer != VK_NULL_HANDLE && VertexCount > 0;
        }
    };

struct FTerrainRenderInfo
    {
    // Vulkan буферы
    VkBuffer VertexBuffer = VK_NULL_HANDLE;
    VkBuffer IndexBuffer = VK_NULL_HANDLE;

    // Количество вершин/индексов
    uint32_t VertexCount = 0;
    uint32_t IndexCount = 0;

    // Матрица трансформации (обычно identity для террейна)
    FMat4 Model = FMat4 ( 1.f );

    // Размеры террейна в ячейках
    int32 Width = 0;
    int32 Height = 0;

    // Размер ячейки в мировых единицах
    float CellSize = 100.0f;

    // Минимальная и максимальная высота
    float MinHeight = 0.0f;
    float MaxHeight = 0.0f;

    // Параметры для шейдера (можно передавать как push constants)
    struct FTerrainParams
        {
        float TilingFactor = 1.0f;      // Множитель UV
        float HeightScale = 1.0f;       // Масштаб высоты
        float FogDensity = 0.001f;      // Плотность тумана
        float UseTexture = 0.0f;         // Использовать текстуры (0/1)

        // Высоты для слоёв текстурирования
        float SandHeight = 0.0f;
        float GrassHeight = 30.0f;
        float RockHeight = 60.0f;
        float SnowHeight = 90.0f;
        } Params;

    // Имя пайплайна для рендера террейна
    std::string PipelineName = "TerrainPipeline";

    // Конструктор
    FTerrainRenderInfo () = default;

    // Проверка валидности
    bool IsValid () const
        {
        return VertexBuffer != VK_NULL_HANDLE && VertexCount > 0 && Width > 0 && Height > 0;
        }

    // Очистка
    void Clear ()
        {
        VertexBuffer = VK_NULL_HANDLE;
        IndexBuffer = VK_NULL_HANDLE;
        VertexCount = 0;
        IndexCount = 0;
        Model = FMat4 ( 1.f );
        Width = 0;
        Height = 0;
        CellSize = 100.0f;
        MinHeight = 0.0f;
        MaxHeight = 0.0f;
        Params = FTerrainParams ();
        }
    };


struct FRenderInfo
    {
    // Флаг наличия данных
    bool HasInfo = false;

    // Камера для текущего кадра
    FCameraInfo Camera;

    // Меши для отрисовки (обычные статические меши)
    std::vector<FMeshInfo> RenderMeshes;

    // НОВОЕ: Специализированные террейны для отрисовки
    std::vector<FTerrainRenderInfo> Terrains;

    // Конструктор
    FRenderInfo () = default;

    // Методы
    void Clear ();

    // Вспомогательные методы
    bool IsEmpty () const { return RenderMeshes.empty () && Terrains.empty (); }
    size_t GetMeshCount () const { return RenderMeshes.size (); }
    size_t GetTerrainCount () const { return Terrains.size (); }

    void Reserve ( size_t MeshCount, size_t TerrainCount = 0 )
        {
        RenderMeshes.reserve ( MeshCount );
        Terrains.reserve ( TerrainCount );
        }

    // Добавление меша
    void AddMesh ( const FMeshInfo & Mesh ) { RenderMeshes.push_back ( Mesh ); }

    // НОВОЕ: Добавление террейна
    void AddTerrain ( const FTerrainRenderInfo & Terrain ) { Terrains.push_back ( Terrain ); }

    // Проверка валидности
    bool IsValid () const { return HasInfo && ( !RenderMeshes.empty () || !Terrains.empty () ); }
    };


// Вспомогательные функции для создания камер
FCameraInfo CreatePerspectiveCamera ( const FVector & Position,
                                      const FVector & Target,
                                      float FOVDegrees,
                                      float AspectRatio,
                                      float Near = 0.1f,
                                      float Far = 1000.f );

FCameraInfo CreateOrthographicCamera ( const FVector & Position,
                                       const FVector & Target,
                                       float Left, float Right,
                                       float Bottom, float Top,
                                       float Near = 0.1f,
                                       float Far = 1000.f );