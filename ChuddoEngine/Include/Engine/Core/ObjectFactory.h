#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>

// Forward declaration
class CObject;

// Simple registration macro
#define REGISTER_CLASS_FACTORY(ClassName) \
    namespace { \
        struct ClassName##Registrar { \
            ClassName##Registrar() { \
                CObjectFactory::GetInstance().RegisterClass<ClassName>(); \
            } \
        }; \
        static ClassName##Registrar ClassName##_AutoReg; \
    }

class CObjectFactory
    {
    public:
        using CreatorFunc = std::function<CObject * ( CObject * owner, const std::string & displayName )>;

        // Singleton pattern
        static CObjectFactory & GetInstance ()
            {
            static CObjectFactory instance;
            return instance;
            }

            // Disable copying
        CObjectFactory ( const CObjectFactory & ) = delete;
        CObjectFactory & operator=( const CObjectFactory & ) = delete;

        // Register a class with automatic name detection
        template<typename T>
        void RegisterClass ()
            {
            static_assert( std::is_base_of_v<CObject, T>,
                           "CObjectFactory: Class must derive from CObject" );

            std::string className = T::StaticClassName ();

            if (IsClassRegistered ( className ))
                {
                std::cerr << "Warning: Class '" << className << "' is already registered!\n";
                return;
                }

                // Creator lambda
            CreatorFunc creator = [] ( CObject * owner, const std::string & displayName ) -> CObject *
                {
                return new T ( owner, displayName );
                };

                // Store creator
            ClassCreators[ className ] = creator;

            // Store class hierarchy info
            ClassHierarchy[ className ] = T::StaticBaseClassName ();

            std::cout << "[FACTORY] Registered class '" << className
                << "' (base: '" << T::StaticBaseClassName () << "')\n";
            }

            // Register a class with custom name
        template<typename T>
        void RegisterClass ( const std::string & className )
            {
            static_assert( std::is_base_of_v<CObject, T>,
                           "CObjectFactory: Class must derive from CObject" );

            if (IsClassRegistered ( className ))
                {
                std::cerr << "Warning: Class '" << className << "' is already registered!\n";
                return;
                }

            ClassCreators[ className ] = [] ( CObject * owner, const std::string & displayName ) -> CObject *
                {
                return new T ( owner, displayName );
                };

            std::cout << "[FACTORY] Registered class '" << className << "'\n";
            }

      // Create object by class name and automatically add to owner
        CObject * Create ( const std::string & className,
                           CObject * owner = nullptr,
                           const std::string & displayName = "Object" )
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

            // Check if class is registered
        bool IsClassRegistered ( const std::string & className ) const
            {
            return ClassCreators.find ( className ) != ClassCreators.end ();
            }

            // Get all registered class names
        std::vector<std::string> GetRegisteredClasses () const
            {
            std::vector<std::string> classes;
            for (const auto & [name, _] : ClassCreators)
                classes.push_back ( name );
            return classes;
            }

            // Get class hierarchy
        std::string GetClassHierarchy () const
            {
            std::stringstream ss;
            ss << "Class Hierarchy:\n";
            for (const auto & [className, baseClass] : ClassHierarchy)
                {
                ss << "  " << className << " -> " << baseClass << "\n";
                }
            return ss.str ();
            }

    private:
        CObjectFactory () = default;
        ~CObjectFactory () = default;

        std::unordered_map<std::string, CreatorFunc> ClassCreators;
        std::unordered_map<std::string, std::string> ClassHierarchy;

        // Find parent class for a given class name
        std::string FindParentClass ( const std::string & className ) const
            {
            auto it = ClassHierarchy.find ( className );
            if (it != ClassHierarchy.end ())
                return it->second;
            return "";
            }
    };

    // Singleton access macro
#define OBJECT_FACTORY CObjectFactory::GetInstance()