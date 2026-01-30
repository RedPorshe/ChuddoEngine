#include "CoreMinimal.h"
#include "Core/Object.h"
#include <iostream>

class CActor : public CObject
    {
    public:
        CActor ( CObject * inOwner = nullptr, const std::string & inName = "Actor" )
            : CObject ( inOwner, inName )
            {
            std::cout << "Created actor: " << this->GetName () << "\n";
            }
    };

int CountObjects ( CObject * root )
    {
    int count = 1;
    for (const auto & child : root->GetOwnedObjects ())
        {
        count += CountObjects ( child.get () );
        }
    return count;
    }

CObject * FindObjectByName ( CObject * root, const std::string & name )
    {
    if (root->GetName () == name)
        return root;

    for (const auto & child : root->GetOwnedObjects ())
        {
        CObject * found = FindObjectByName ( child.get (), name );
        if (found)
            return found;
        }
    return nullptr;
    }

void PrintHierarchy ( CObject * obj, int depth )
    {
    std::string indent ( depth * 2, ' ' );
    std::cout << indent << "- " << obj->GetName ()
        << " (" << typeid( *obj ).name () << ")";

    if (obj->HasOwner ())
        {
        std::cout << " [Owner: " << obj->GetOwner ()->GetName () << "]";
        }
    std::cout << "\n";

    for (const auto & child : obj->GetOwnedObjects ())
        {
        PrintHierarchy ( child.get (), depth + 1 );
        }
    }

int main ()
    {
    std::cout << "=== CHUDDO ENGINE DEMO v4 ===\n\n";

    // 1. Создаем мир
    auto world = std::make_unique<CObject> ( nullptr, "World" );
    std::cout << "Created: " << world->GetName () << "\n\n";

    // 2. Добавляем игроков
    auto player = world->AddSubObject<CActor> ( "Player" );
    auto enemy = world->AddSubObject<CActor> ( "Enemy" );

    // 3. Добавляем компоненты игроку
    if (player)
        {
        auto transform = player->AddSubObject<CObject> ( "Transform" );
        auto camera = player->AddSubObject<CObject> ( "Camera" );
        auto inventory = player->AddSubObject<CObject> ( "Inventory" );

        // Создаем оружие в инвентаре
        if (inventory)
            {
            inventory->AddSubObject<CObject> ( "Sword" );
            inventory->AddSubObject<CObject> ( "Shield" );
            inventory->AddSubObject<CObject> ( "Potion" );
            }
        }

        // 4. Выводим иерархию
    std::cout << "\n=== OBJECT HIERARCHY ===\n";
    PrintHierarchy ( world.get (), 0 );

    // 5. Статистика
    std::cout << "\n=== STATISTICS ===\n";
    std::cout << "Total objects in world: " << CountObjects ( world.get () ) << "\n";
    std::cout << "World has " << world->GetNumOwnedObjects () << " direct children\n";

    // 6. Поиск объектов
    std::cout << "\n=== SEARCH ===\n";
    CObject * found = FindObjectByName ( world.get (), "Sword" );
    if (found)
        {
        std::cout << "Found Sword! Owner: "
            << ( found->GetOwner () ? found->GetOwner ()->GetName () : "None" )
            << "\n";
        }

        // 7. Удаление объекта
    std::cout << "\n=== REMOVAL ===\n";
    if (player && player->RemoveOwnedObject ( "Camera" ))
        {
        std::cout << "Camera component removed\n";
        }

        // 8. Переименование (ИСПРАВЛЕНО!)
    std::cout << "\n=== RENAME ===\n";
    if (enemy)
        {
            // Правильный вызов - указываем старое и новое имя
        if (world->RenameOwnedObject ( "Enemy", "Enemy_Boss" ))
            {
            std::cout << "Enemy successfully renamed to Enemy_Boss\n";
            }
        }

        // 9. Тест на дублирование имен
    std::cout << "\n=== DUPLICATE NAME TEST ===\n";
    auto duplicate = world->AddSubObject<CObject> ( "Player" );
    if (!duplicate)
        {
        std::cout << "✓ Correctly rejected duplicate name 'Player'\n";
        }

        // 10. Тест на циклические ссылки
    std::cout << "\n=== CYCLIC REFERENCE TEST ===\n";
    std::cout << "Trying to create object with parent's name...\n";
    auto badName = player->AddSubObject<CObject> ( "Player" );
    if (!badName)
        {
        std::cout << "✓ Correctly prevented object with parent's name\n";
        }

        // 11. Тест клонирования
    std::cout << "\n=== CLONING TEST ===\n";
    auto worldClone = world->Clone ();
    if (worldClone)
        {
        std::cout << "✓ World cloned successfully\n";
        std::cout << "Clone name: " << worldClone->GetName () << "\n";
        std::cout << "Clone has " << worldClone->GetNumOwnedObjects ()
            << " direct children\n";
        }

        // 12. Тест передачи владения
    std::cout << "\n=== OWNERSHIP TRANSFER TEST ===\n";
    auto tempObj = std::make_unique<CObject> ( nullptr, "TempObject" );
    if (world->TransferOwnership ( player, tempObj.get () ))
        {
        std::cout << "✓ Ownership transferred\n";
        }
    else
        {
        std::cout << "✗ Ownership transfer failed (expected - player not owned by world)\n";
        }

        // 13. Финальная иерархия
    std::cout << "\n=== FINAL HIERARCHY ===\n";
    PrintHierarchy ( world.get (), 0 );

    std::cout << "\n=== CLEANUP ===\n";
    return 0;
    }