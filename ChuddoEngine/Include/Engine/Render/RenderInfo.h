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

    // Конструктор
    FMeshInfo () = default;

    FMeshInfo ( VkBuffer InVertexBuffer, uint32_t InVertexCount,
                VkBuffer InIndexBuffer = VK_NULL_HANDLE, uint32_t InIndexCount = 0,
                const FMat4 & InModel = FMat4 ( 1.f ) )
        : Model ( InModel )
        , VertexBuffer ( InVertexBuffer )
        , IndexBuffer ( InIndexBuffer )
        , VertexCount ( InVertexCount )
        , IndexCount ( InIndexCount )
        , MaterialId ( 0 )
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
        }

    // Проверка валидности
    bool IsValid () const
        {
        return VertexBuffer != VK_NULL_HANDLE && VertexCount > 0;
        }
    };

struct FRenderInfo
    {
    // Флаг наличия данных
    bool HasInfo = false;

    // Камера для текущего кадра
    FCameraInfo Camera;

    // Меши для отрисовки
    std::vector<FMeshInfo> RenderMeshes;

    // Конструктор
    FRenderInfo () = default;

    // Методы
    void Clear ();

    // Вспомогательные методы
    bool IsEmpty () const { return RenderMeshes.empty (); }
    size_t GetMeshCount () const { return RenderMeshes.size (); }
    void Reserve ( size_t Count ) { RenderMeshes.reserve ( Count ); }

    // Добавление меша
    void AddMesh ( const FMeshInfo & Mesh ) { RenderMeshes.push_back ( Mesh ); }

    // Проверка валидности
    bool IsValid () const { return HasInfo && !RenderMeshes.empty (); }
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