#include "Core/ObjectFactory.h"
#include "Core/Object.h"

CObject * CObjectFactory::Create ( const std::string & className,
								   CObject * owner,
								   const std::string & displayName )
	{
		// Проверка валидности имени
	if (displayName.empty ())
		{
		LOG_ERROR ( "[FACTORY] ERROR: Display name cannot be empty!" );
		return nullptr;
		}

		// Проверить уникальность имени в иерархии owner
	std::string finalDisplayName = displayName;
	if (owner)
		{
		CObject * root = owner->GetRoot ();
		while (root->GetOwner () != nullptr)
			{
			root = root->GetOwner ();
			LOG_DEBUG ( "finding final root" );
			}
		if (root->FindRecursive ( finalDisplayName ))
			{
				// Генерируем уникальное имя
			finalDisplayName = CObject::GenerateUniqueDisplayNameVariant ( finalDisplayName, root );
			LOG_DEBUG ( "[FACTORY] Note: Display name '", displayName
						, "' already exists, using '", finalDisplayName, "' instead" );
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

			LOG_DEBUG ( "[FACTORY] Created '", finalDisplayName
						, "' of type '", className, "'" );
			}
		return obj;
		}

		// Try to find in parent classes (for backward compatibility)
	std::string parentClass = FindParentClass ( className );
	if (!parentClass.empty ())
		{
		LOG_DEBUG ( "[FACTORY] Class '", className
					, "' not found, using parent '", parentClass, "'" );
		return Create ( parentClass, owner, finalDisplayName );
		}

	LOG_ERROR ( "[FACTORY] ERROR - Unknown class type: '", className, "'" );
	LOG_ERROR ( "[FACTORY] Available classes: " );
	for (const auto & [name, _] : ClassCreators)
		LOG_ERROR ( name );
	return nullptr;
	}