#pragma once
#include "CoreMinimal.h"

class CObject
    {
    public:
        CObject ( CObject * inOwner = nullptr, const std::string & inName = "Object" );
        virtual ~CObject ();

        CObject * GetOwner () const { return ObjectOwner; }
        bool HasOwner () const { return GetOwner () != nullptr; } // ИСПРАВЛЕНО: != nullptr
        std::string GetName () const { return ObjectName; }

        CObject * FindOwned ( const std::string & name ) const;
        const std::vector<std::unique_ptr<CObject>> & GetOwnedObjects () const { return OwnedObjects; }
        size_t GetNumOwnedObjects () const { return OwnedObjects.size (); }
        bool RemoveOwnedObject ( const std::string & name );

        void AddOwnedObject ( std::unique_ptr<CObject> object );
        void AddOwnedObject ( CObject * object );
        bool TransferOwnership ( CObject * obj, CObject * newOwner );

        template<typename ClassName, typename... Args>
        ClassName * AddSubObject ( const std::string & name = "SubObject", Args&&... args )
            {
            static_assert( std::is_base_of<CObject, ClassName>::value,
                           "Class must be derived from CObject" );

              // 1. Проверка на пустое имя
            if (name.empty ())
                {
                std::cerr << "Error: Object name cannot be empty!\n";
                return nullptr;
                }

                // 2. Проверка на дублирование имени
            if (FindOwned ( name ))
                {
                std::cerr << "Error: Object with name '" << name
                    << "' already exists!" << std::endl;
                return nullptr;
                }

                // 3. Проверка на циклическую ссылку (нельзя добавить себя)
            if (name == GetName ())
                {
                std::cerr << "Error: Cannot create object with same name as parent!\n";
                return nullptr;
                }

                // 4. Проверка на добавление предка (циклическая ссылка)
            CObject * current = GetOwner ();
            while (current)
                {
                if (current->GetName () == name)
                    {
                    std::cerr << "Error: Would create cyclic reference with ancestor '"
                        << name << "'!\n";
                    return nullptr;
                    }
                current = current->GetOwner ();
                }

                // Создаем объект
            auto newObj = std::make_unique<ClassName> ( this, name, std::forward<Args> ( args )... );
            ClassName * rawPtr = newObj.get ();

            // Сохраняем владение
            AddOwnedObject ( std::move ( newObj ) );

            return rawPtr;
            }

        bool RenameOwnedObject ( const std::string & oldName, const std::string & newName );

        template<typename T>
        T * FindOwnedAs ( const std::string & name ) const
            {
            CObject * obj = FindOwned ( name );
            if (obj)
                {
                return dynamic_cast< T * >( obj );
                }
            return nullptr;
            }

        std::unique_ptr<CObject> Clone () const;

        // Новый метод: переименовать себя
        bool Rename ( const std::string & newName );
        

    protected:
        CObject * ObjectOwner = nullptr;
        std::string ObjectName {};
        std::vector<std::unique_ptr<CObject>> OwnedObjects;
    };