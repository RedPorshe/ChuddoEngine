#include "Core/Object.h"

CObject::CObject ( CObject * inOwner, const std::string & inName )
    : ObjectOwner ( inOwner ), ObjectName ( inName )
    {
    //    // Автоматически добавляем себя к владельцу
    //if (inOwner)
    //    {
    //    inOwner->AddOwnedObject ( this );
    //    }
    }

CObject::~CObject ()
    {
    std::cout << typeid( *this ).name () << " named '"
        << this->GetName () << "' Destruction started\n";
    }

CObject * CObject::FindOwned ( const std::string & name ) const
    {
    for (size_t i = 0; i < OwnedObjects.size (); i++)
        {
        if (OwnedObjects[ i ].get ()->GetName () == name)
            {
            return OwnedObjects[ i ].get ();
            }
        }
    return nullptr;
    }

bool CObject::RemoveOwnedObject ( const std::string & name )
    {
    auto it = std::find_if ( OwnedObjects.begin (), OwnedObjects.end (),
                             [ &name ] ( const std::unique_ptr<CObject> & obj )
                             {
                             return obj && obj->GetName () == name;
                             } );

    if (it != OwnedObjects.end ())
        {
        std::cout << ( *it )->GetName () << " removed from " << GetName () << "\n";
        OwnedObjects.erase ( it );
        return true;
        }
    return false;
    }

void CObject::AddOwnedObject ( std::unique_ptr<CObject> object )
    {
    if (!object || object.get () == this) return;

    object->ObjectOwner = this;
    OwnedObjects.push_back ( std::move ( object ) );
    }

void CObject::AddOwnedObject ( CObject * object )
    {
    if (!object || object == this) return;

    auto uniquePtr = std::unique_ptr<CObject> ( object );
    AddOwnedObject ( std::move ( uniquePtr ) );
    }

bool CObject::TransferOwnership ( CObject * obj, CObject * newOwner )
    {
    if (!obj || !newOwner || obj == newOwner || obj == this) return false;

    auto it = std::find_if ( OwnedObjects.begin (), OwnedObjects.end (),
                             [ obj ] ( const std::unique_ptr<CObject> & owned )
                             {
                             return owned.get () == obj;
                             } );

    if (it != OwnedObjects.end ())
        {
        std::unique_ptr<CObject> temp = std::move ( *it );
        OwnedObjects.erase ( it );

        temp->ObjectOwner = newOwner;
        newOwner->AddOwnedObject ( std::move ( temp ) );

        std::cout << "Transferred '" << obj->GetName ()
            << "' from '" << GetName ()
            << "' to '" << newOwner->GetName () << "'\n";
        return true;
        }
    return false;
    }

bool CObject::RenameOwnedObject ( const std::string & oldName, const std::string & newName )
    {
    if (oldName.empty ())
        {
        std::cerr << "Error: Old name cannot be empty!\n";
        return false;
        }

    if (newName.empty ())
        {
        std::cerr << "Error: New name cannot be empty!\n";
        return false;
        }

        // Проверяем, что новое имя свободно
    if (FindOwned ( newName ))
        {
        std::cerr << "Error: Name '" << newName << "' already in use!\n";
        return false;
        }

    CObject * obj = FindOwned ( oldName );
    if (obj)
        {
        return obj->Rename ( newName ); // Используем метод Rename объекта
        }

    std::cerr << "Error: Object '" << oldName << "' not found!\n";
    return false;
    }

std::unique_ptr<CObject> CObject::Clone () const
    {
    auto clone = std::make_unique<CObject> ( nullptr, GetName () + "_Copy" );

    for (const auto & child : OwnedObjects)
        {
        auto childClone = child->Clone ();
        clone->AddOwnedObject ( std::move ( childClone ) );
        }

    return clone;
    }

bool CObject::Rename ( const std::string & newName )
    {
    if (newName.empty ())
        {
        std::cerr << "Error: New name cannot be empty!\n";
        return false;
        }

        // Проверяем у владельца, нет ли уже объекта с таким именем
    if (ObjectOwner && ObjectOwner->FindOwned ( newName ))
        {
        std::cerr << "Error: Name '" << newName
            << "' already exists in owner's hierarchy!\n";
        return false;
        }

    std::string oldName = ObjectName;
    ObjectName = newName;
    std::cout << "Renamed '" << oldName << "' to '" << newName << "'\n";
    return true;
    }
