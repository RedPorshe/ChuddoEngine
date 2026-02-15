# Использование ChuddoEngine (краткое руководство)

Этот файл объясняет, как использовать движок в текущем репозитории: сборка, создание мира/левелов, добавление акторов и компонентов, базовые примеры по системе коллизий и использование математических утилит `CEMath`.

1) Сборка и запуск
- Откройте `ChuddoEngine.sln` в Visual Studio 2022/2026 (если он отсутствует, создайте проект в VS и добавьте исходники).
- Постройте конфигурацию `Debug` или `Release`.
- Запустите исполняемый файл; `Source/main.cpp` уже содержит пример инициализации движка.

2) Создание актора и добавление компонентов
- Создайте класс, производный от `CActor`, и в конструкторе добавьте компоненты через `AddDefaultSubObject<T>("Name")`.

Пример:
```cpp
class CMyActor : public CActor
{
public:
    CMyActor()
    {
        auto cam = AddDefaultSubObject<CCameraComponent>("Camera");
        auto sphere = AddDefaultSubObject<CSphereComponent>("SphereCollision");
        sphere->SetRadius(50.0f);
        sphere->SetChannelAsDynamic();
        sphere->SetResponseToChannel("Static", ECollisionResponse::BLOCK);
    }
};
```

3) Коллизии
- Используйте `CCollisionSystem::Get()` (или глобальную переменную `COLLISION_SYSTEM`) для Raycast/Overlap тестов.
- Подписывайтесь на события в акторе: `OnComponentBeginOverlap`, `OnComponentEndOverlap`, `OnComponentHit`.

4) Взаимодействие с физикой/гравитацией
- Добавьте `CGravityComponent` и настройте силу гравитации и режимы.

5) Тесты
- Включены тестовые уровни в `Source/Tests` (если присутствуют). Запустите из кода или добавьте простую сцену в `main.cpp`.

6) Полезные команды
- `LOG_DEBUG(...)`, `LOG_INFO(...)`, `LOG_ERROR(...)` — макросы логирования.

7) Создание мира и левелов
-- Инициализация игрового инстанса и создание мира/левела обычно выполняется движком, но можно сделать это в коде вручную:
```cpp
// Создать синглтон GameInstance (если ещё не создан)
CGameInstance::Create();
auto & gameInstance = CGameInstance::Get();

// Создать World
CWorld * world = gameInstance.CreateWorld("MainWorld");

// Создать Level (шаблонный метод создаёт объект и добавляет в World)
// Можно передать свой класс, унаследованный от CLevel
CLevel * level = world->CreateLevel<CLevel>("TestLevel");

// Установить текущий уровень (опционально)
world->SetCurrentLevel(level);

// Спавн акторов в уровне (если класс акторов унаследован от CActor)
// Метод SpawnActor<T> создаёт актор сразу или ставит в очередь
auto * newActor = level->SpawnActor<CActor>("MyActor");

// Создание террейна
CTerrainActor * terrain = level->SpawnTerrainActor("Terrain", 128, 128, 100.0f, 0.0f);
// Или из heightmap
// std::vector<float> heights = ...; level->SpawnTerrainActorFromHeightmap("TerrainHM", heights, w, h, cellSize);

// Запуск жизненного цикла уровня вручную (обычно вызывает движок)
level->BeginPlay();
// в основном цикле: level->Tick(deltaTime);
// по завершении: level->EndPlay();
```

Полезные методы в `CLevel`:
- `SpawnActor<T>(name)` — шаблонный спавн акторов (поддерживает отложенный спавн через очередь).
- `SpawnTerrainActor(...)` / `SpawnTerrainActorFromHeightmap(...)` — создание террейна.
- `ProcessSpawnQueue()` / `ProcessAllPendingSpawns()` — обработка очереди спавна.

8) Использование CEMath (Math helpers и типы)
-- Основные типы и алиасы находятся в `Include/Utils/Math/MathTypes.h`:
   - `FVector` — трёхмерный вектор (alias для `CEMath::Vec3`)
   - `FQuat`, `FMat4`, `FTransform` и т.д.

Примеры:
```cpp
#include "Utils/Math/MathTypes.h"

// Векторы и линтерп
FVector a(0.0f, 0.0f, 0.0f);
FVector b(100.0f, 0.0f, 0.0f);
// статический метод Lerp доступен в классе Vector3D
FVector mid = FVector::Lerp(a, b, 0.5f);

// Нормализация и длина
FVector dir = (b - a).Normalized();
float len = dir.Length();

// Утилиты из CEMath
float angleRad = CEMath::DegreesToRadians(90.0f);
float wrapped = CEMath::WrapAngleDegrees(270.0f);

// Кватернионы и поворот
FQuat q = FQuat::FromEulerAngles(0.0f, angleRad, 0.0f);
FVector rotated = q * FVector::UnitX();

// FTransform удобен для комбинирования трансформаций
FTransform t(FVector(0, 0, 0), q, FVector::One());
FVector worldPos = t.Location + (t.Rotation * FVector::UnitZ());
```

Советы
- Откройте `Include/Utils/Math` для полного списка методов (`Vector3D`, `Quaternion`, `Matrix4` и т.д.).
- Используйте `CEMath::Clamp`, `Lerp`, `SmoothStep` и т.п. для интерполяций и ограничений.

Если нужна подробная документация по конкретным методам или примеры использования конкретных компонентов (террейн, капсула, raycast) — напишите, добавлю примеры.

9) Требования при объявлении классов и регистрация
- Все классы, которые должны участвовать в фабрике/сериализации и иметь поддержку создания через `AddSubObject` или `AddSubObjectByClass`, должны:
  - В теле класса использовать макрос `CHUDDO_DECLARE_CLASS(YourClass, BaseClass)` (обычно в заголовочном файле). Это добавляет алиас `Super`, статические методы имени класса и базовые методы сериализации.
  - Иметь конструктор с сигнатурой `YourClass(CObject* owner = nullptr, const std::string & inName = "YourName")` и обязательно вызвать конструктор базового класса в списке инициализации (`: Super(owner, inName)` или `: CObject(owner, inName)`).
  - В конце объявления/реализации класса вызвать макрос `REGISTER_CLASS_FACTORY(YourClass);` чтобы класс был зарегистрирован в `CObjectFactory` при запуске.

Пример заголовочного файла:
```cpp
// MyActor.h
class CMyActor : public CActor
{
    CHUDDO_DECLARE_CLASS(CMyActor, CActor)
public:
    CMyActor(CObject* owner = nullptr, const std::string& inName = "MyActor");
    ~CMyActor();
};

REGISTER_CLASS_FACTORY(CMyActor);
```

И пример реализации конструктора:
```cpp
// MyActor.cpp
#include "MyActor.h"

CMyActor::CMyActor(CObject* owner, const std::string& inName)
    : Super(owner, inName) // обязательно вызвать базовый конструктор
{
    // инициализация компонентов
    auto root = AddDefaultSubObject<CTransformComponent>(GetName() + "_Transform");
}

CMyActor::~CMyActor() {}
```

Пояснения:
- Макрос `CHUDDO_DECLARE_CLASS` упрощает интеграцию с фабрикой и системой сериализации.
- Конструктор должен принимать `CObject* owner` и `const std::string& inName` с значениями по умолчанию — это позволяет фабрике и `AddSubObject` корректно создавать экземпляры и задавать имя.
- `REGISTER_CLASS_FACTORY` вызывает регистрацию класса в `CObjectFactory` (обеспечивает создание по имени класса — важно для динамического создания и десериализации).

Соблюдение этих правил обеспечит корректную работу создания объектов через фабрику, владения объектов и сериализации.
