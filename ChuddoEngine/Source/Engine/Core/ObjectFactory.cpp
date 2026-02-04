#include "Core/ObjectFactory.h"
#include "Core/Object.h"

CObject * CObjectFactory::Create ( const std::string & className,
                                   CObject * owner,
                                   const std::string & displayName )
    {
        // Проверка валидности имени
    if (displayName.empty ())
        {
        std::cerr << "[FACTORY] ERROR: Display name cannot be empty!\n";
        return nullptr;
        }

        // Проверить уникальность имени в иерархии owner
    std::string finalDisplayName = displayName;
    if (owner)
        {
        CObject * root = owner->GetRoot ();
        if (root->FindRecursive ( finalDisplayName ))
            {
                // Генерируем уникальное имя
            finalDisplayName = CObject::GenerateUniqueDisplayNameVariant ( finalDisplayName, root );
            std::cout << "[FACTORY] Note: Display name '" << displayName
                << "' already exists, using '" << finalDisplayName << "' instead\n";
            }
        }

    auto it = ClassCreators.find ( className );
    if (it != ClassCreators.end ())
        {
        CObject * obj = it->second ( owner, finalDisplayName );
        if (obj)
            {
                // АВТОМАТИЧЕСКИ добавляем к владельцу, если он указан
            if (owner)
                {
                owner->AddOwnedObject ( obj );
                }

            std::cout << "[FACTORY] Created '" << finalDisplayName
                << "' of type '" << className << "'\n";
            }
        return obj;
        }

        // Try to find in parent classes (for backward compatibility)
    std::string parentClass = FindParentClass ( className );
    if (!parentClass.empty ())
        {
        std::cout << "[FACTORY] Class '" << className
            << "' not found, using parent '" << parentClass << "'\n";
        return Create ( parentClass, owner, finalDisplayName );
        }

    std::cerr << "[FACTORY] ERROR - Unknown class type: '" << className << "'\n";
    std::cerr << "[FACTORY] Available classes: ";
    for (const auto & [name, _] : ClassCreators)
        std::cerr << name << " ";
    std::cerr << "\n";

    return nullptr;
    }