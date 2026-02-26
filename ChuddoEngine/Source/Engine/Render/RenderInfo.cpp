#include "Render/RenderInfo.h"
#include "Utils/Math/CE_MathConstants.h"
#include "Utils/Math/CE_MathHelpers.h"

//=============================================================================
// FCameraInfo Implementation
//=============================================================================

FCameraInfo::FCameraInfo ( const FVector & inLocation,
                           const FVector & inViewTarget,
                           const FMat4 & inViewMatrix,
                           const FMat4 & inProjectionMatrix,
                           const float & inNearPlane,
                           const float & inFarPlane,
                           const float & inFOVAngles )
    : ViewMatrix ( inViewMatrix )
    , ProjectionMatrix ( inProjectionMatrix )
    , Location ( inLocation )
    , ViewTarget ( inViewTarget )
    , NearPlane ( inNearPlane )
    , FarPlane ( inFarPlane )
    , FOV ( inFOVAngles )
    {}

void FCameraInfo::Clear ()
    {
    ViewMatrix = FMat4::IdentityMatrix ();
    ProjectionMatrix = FMat4::IdentityMatrix ();
    Location = FVector::Zero ();
    ViewTarget = FVector::Zero ();
    NearPlane = 0.1f;
    FarPlane = 1000.f;
    FOV = 90.f;
    }

void FCameraInfo::UpdateViewMatrix ()
    {
    // Вычисление LookAt матрицы с использованием вашей математики
    FVector Direction = ( ViewTarget - Location );
    if (!Direction.IsZero ())
        Direction.Normalize ();

    // Вектор вверх по умолчанию (Y-up)
    FVector Up ( 0.0f, 1.0f, 0.0f );

    // Вычисление правого вектора
    FVector Right = Up.Cross ( Direction );
    if (!Right.IsZero ())
        Right.Normalize ();
    else
        Right = FVector ( 1.0f, 0.0f, 0.0f ); // Fallback если up и direction коллинеарны

    // Пересчет вектора вверх для ортогональности
    FVector AdjustedUp = Direction.Cross ( Right );
    AdjustedUp.Normalize ();

    // Построение матрицы вида (column-major для Vulkan)
    // Первая колонка - Right
    ViewMatrix ( 0, 0 ) = Right.x;
    ViewMatrix ( 1, 0 ) = Right.y;
    ViewMatrix ( 2, 0 ) = Right.z;
    ViewMatrix ( 3, 0 ) = -Right.Dot ( Location );

    // Вторая колонка - AdjustedUp
    ViewMatrix ( 0, 1 ) = AdjustedUp.x;
    ViewMatrix ( 1, 1 ) = AdjustedUp.y;
    ViewMatrix ( 2, 1 ) = AdjustedUp.z;
    ViewMatrix ( 3, 1 ) = -AdjustedUp.Dot ( Location );

    // Третья колонка - Direction
    ViewMatrix ( 0, 2 ) = Direction.x;
    ViewMatrix ( 1, 2 ) = Direction.y;
    ViewMatrix ( 2, 2 ) = Direction.z;
    ViewMatrix ( 3, 2 ) = -Direction.Dot ( Location );

    // Четвертая колонка - translation
    ViewMatrix ( 0, 3 ) = 0.0f;
    ViewMatrix ( 1, 3 ) = 0.0f;
    ViewMatrix ( 2, 3 ) = 0.0f;
    ViewMatrix ( 3, 3 ) = 1.0f;
    }

void FCameraInfo::UpdateProjectionMatrix ( float AspectRatio )
    {
    // Проверка на корректность AspectRatio
    if (AspectRatio <= 0.0f)
        AspectRatio = 16.0f / 9.0f;

    // Конвертация FOV из градусов в радианы (используем ваши константы)
    float RadFOV = FOV * CEMath::DEG_TO_RAD;

    // Вычисление параметров перспективной проекции
    float tanHalfFOV = CEMath::Tan ( RadFOV * 0.5f );
    float range = NearPlane - FarPlane;

    // Построение перспективной матрицы проекции (column-major для Vulkan)
    // Перспективная проекция с глубиной от 0 до 1 (для Vulkan)
    ProjectionMatrix = FMat4 (
        1.0f / ( AspectRatio * tanHalfFOV ), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tanHalfFOV, 0.0f, 0.0f,
        0.0f, 0.0f, ( FarPlane + NearPlane ) / range, -1.0f,
        0.0f, 0.0f, ( 2.0f * FarPlane * NearPlane ) / range, 0.0f
    );

    // Альтернатива для OpenGL стиля (глубина от -1 до 1):
    // ProjectionMatrix = FMat4(
    //     1.0f / (AspectRatio * tanHalfFOV), 0.0f, 0.0f, 0.0f,
    //     0.0f, 1.0f / tanHalfFOV, 0.0f, 0.0f,
    //     0.0f, 0.0f, -(FarPlane + NearPlane) / (FarPlane - NearPlane), -1.0f,
    //     0.0f, 0.0f, -(2.0f * FarPlane * NearPlane) / (FarPlane - NearPlane), 0.0f
    // );
    }

//=============================================================================
// FRenderInfo Implementation
//=============================================================================

void FRenderInfo::Clear ()
    {
    HasInfo = false;
    Camera.Clear ();
    RenderMeshes.clear ();
    Terrains.clear ();
    }

//=============================================================================
// Вспомогательные функции
//=============================================================================

FCameraInfo CreatePerspectiveCamera ( const FVector & Position,
                                      const FVector & Target,
                                      float FOVDegrees,
                                      float AspectRatio,
                                      float Near,
                                      float Far )
    {
    FCameraInfo Camera;
    Camera.SetLocation ( Position );
    Camera.SetViewTarget ( Target );
    Camera.SetFOV ( FOVDegrees );
    Camera.SetNearPlane ( Near );
    Camera.SetFarPlane ( Far );

    Camera.UpdateViewMatrix ();
    Camera.UpdateProjectionMatrix ( AspectRatio );

    return Camera;
    }

FCameraInfo CreateOrthographicCamera ( const FVector & Position,
                                       const FVector & Target,
                                       float Left, float Right,
                                       float Bottom, float Top,
                                       float Near,
                                       float Far )
    {
    FCameraInfo Camera;
    Camera.SetLocation ( Position );
    Camera.SetViewTarget ( Target );
    Camera.SetNearPlane ( Near );
    Camera.SetFarPlane ( Far );

    // Обновляем матрицу вида
    Camera.UpdateViewMatrix ();

    float Width = Right - Left;
    float Height = Top - Bottom;
    float Depth = Far - Near;

    // Ортографическая матрица проекции (column-major для Vulkan)
    Camera.ProjectionMatrix = FMat4 (
        2.0f / Width, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / Height, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / Depth, 0.0f,
        -( Right + Left ) / Width, -( Top + Bottom ) / Height, -Near / Depth, 1.0f
    );

    return Camera;
    }