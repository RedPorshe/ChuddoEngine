#include "Core/Object.h"
#include "Core/ObjectFactory.h"
#include <functional>
#include <sstream>
#include <cctype>
#include <algorithm>

// Регистрируем CObject в фабрике
namespace
    {
    struct CObjectRegistrar
        {
        CObjectRegistrar () {
            CObjectFactory::GetInstance ().RegisterClass<CObject> ();
            }
        };
    static CObjectRegistrar CObject_AutoReg;
    }

CObject::CObject ( CObject * inOwner, const std::string & inDisplayName )
    : ObjectOwner ( inOwner ), DisplayName ( inDisplayName )
    {
        // Generate unique UUID
    ObjectUUID = GenerateUUID ();

    std::cout << "Created object: " << DisplayName
        << " [UUID: " << GetShortUUID ( ObjectUUID ) << "]\n";
    }

CObject::~CObject ()
    {
    std::cout << GetObjectClassName () << " named '"
        << DisplayName << "' [UUID: " << GetShortUUID ( ObjectUUID )
        << "] Destruction started\n";
    }

void CObject::UpdateDebugIdentifier ()
    {
        // Already handled in GetUniqName()
    }

CObject * CObject::FindOwned ( const std::string & displayName ) const
    {
    for (const auto & obj : OwnedObjects)
        {
        if (obj->GetName () == displayName)
            {
            return obj.get ();
            }
        }
    return nullptr;
    }

bool CObject::FindRecursive ( const std::string & displayName )
    {
    if (GetName () == displayName)
        {
        std::cout << "[DEBUG] FindRecursive: FOUND '" << displayName
            << "' at '" << GetName () << "'\n";
        return true;
        }

    for (const auto & child : OwnedObjects)
        {
        if (child->FindRecursive ( displayName ))
            {
            return true;
            }
        }

    return false;
    }

CObject * CObject::FindObjectByDisplayNameRecursive ( const std::string & displayName )
    {
    auto FoundChild = FindOwned ( displayName );
    if (FoundChild)
        {
        return FoundChild;
        }

        // Search up the hierarchy
    auto root = this;
    while (root->GetOwner ())
        {
        root = root->GetOwner ();
        FoundChild = root->FindOwned ( displayName );
        if (FoundChild)
            return FoundChild;
        }

    return nullptr;
    }

CObject * CObject::FindByUUID ( const std::string & uuid ) const
    {
    if (ObjectUUID == uuid)
        return const_cast< CObject * >( this );

    return nullptr;
    }

CObject * CObject::FindByUUIDRecursive ( const std::string & uuid )
    {
    if (ObjectUUID == uuid)
        return this;

    for (const auto & child : OwnedObjects)
        {
        CObject * found = child->FindByUUIDRecursive ( uuid );
        if (found)
            return found;
        }

    return nullptr;
    }

bool CObject::RemoveOwnedObject ( const std::string & displayName )
    {
    auto it = std::find_if ( OwnedObjects.begin (), OwnedObjects.end (),
                             [ &displayName ] ( const std::unique_ptr<CObject> & obj )
                             {
                             return obj && obj->GetName () == displayName;
                             } );

    if (it != OwnedObjects.end ())
        {
        std::cout << ( *it )->GetName () << " [UUID: "
            << GetShortUUID ( ( *it )->ObjectUUID )
            << "] removed from " << GetName () << "\n";
        OwnedObjects.erase ( it );
        return true;
        }
    return false;
    }

void CObject::AddOwnedObject ( std::unique_ptr<CObject> object )
    {
    if (!object || object.get () == this)
        return;

    object->ObjectOwner = this;
    OwnedObjects.push_back ( std::move ( object ) );
    }

void CObject::AddOwnedObject ( CObject * object )
    {
    if (!object || object == this)
        return;

    auto uniquePtr = std::unique_ptr<CObject> ( object );
    AddOwnedObject ( std::move ( uniquePtr ) );
    }

CObject * CObject::AddSubObjectByClass ( const std::string & className,
                                         const std::string & desiredDisplayName )
    {
    if (desiredDisplayName.empty ())
        {
        std::cerr << "Error: Object display name cannot be empty!\n";
        return nullptr;
        }

        // Find hierarchy root for global check
    CObject * root = this;
    while (root->GetOwner ())
        {
        root = root->GetOwner ();
        }

    std::string finalDisplayName = desiredDisplayName;

    // Check if display name already exists globally
    if (root->FindRecursive ( desiredDisplayName ))
        {
            // Display name exists in hierarchy, generate unique variant
        finalDisplayName = GenerateUniqueDisplayNameVariant ( desiredDisplayName, root );
        std::cout << "Note: Display name '" << desiredDisplayName
            << "' already exists, using '" << finalDisplayName << "' instead\n";
        }

        // Use factory to create object
    CObject * newObj = OBJECT_FACTORY.Create ( className, this, finalDisplayName );
   

    return newObj;
    }

bool CObject::TransferOwnership ( CObject * obj, CObject * newOwner )
    {
    if (!obj || !newOwner || obj == newOwner || obj == this)
        return false;

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
            << "' [UUID: " << GetShortUUID ( obj->ObjectUUID )
            << "] from '" << GetName ()
            << "' to '" << newOwner->GetName () << "'\n";
        return true;
        }
    return false;
    }

    // Helper function to collect similar display names
void CObject::CollectSimilarDisplayNames ( CObject * node, const std::string & baseDisplayName,
                                           std::vector<std::string> & result )
    {
    if (!node) return;

    std::string name = node->GetName ();

    // Check if name starts with baseDisplayName
    if (name.find ( baseDisplayName ) == 0)
        {
            // Exact match
        if (name == baseDisplayName)
            {
            result.push_back ( name );
            }
            // Name in format "baseDisplayName_number"
        else if (name.size () > baseDisplayName.size () && name[ baseDisplayName.size () ] == '_')
            {
            std::string suffix = name.substr ( baseDisplayName.size () + 1 );

            // Check if suffix is a number
            bool isNumber = !suffix.empty ();
            for (char c : suffix)
                {
                if (!std::isdigit ( static_cast< unsigned char >( c ) ))
                    {
                    isNumber = false;
                    break;
                    }
                }

            if (isNumber)
                {
                result.push_back ( name );
                }
            }
        }

        // Recursively check children
    for (const auto & child : node->GetOwnedObjects ())
        {
        CollectSimilarDisplayNames ( child.get (), baseDisplayName, result );
        }
    }

    // Generate unique display name variant
std::string CObject::GenerateUniqueDisplayNameVariant ( const std::string & baseDisplayName, CObject * root )
    {
    std::vector<std::string> similarNames;
    CollectSimilarDisplayNames ( root, baseDisplayName, similarNames );

    // If no similar names, baseDisplayName is available
    if (similarNames.empty ())
        {
        return baseDisplayName;
        }

        // Extract numbers from names
    std::vector<int> usedNumbers;
    bool baseNameExists = false;

    for (const auto & name : similarNames)
        {
        if (name == baseDisplayName)
            {
            baseNameExists = true;
            usedNumbers.push_back ( 0 ); // Base name is number 0
            }
        else
            {
            std::string suffix = name.substr ( baseDisplayName.size () + 1 );
            try
                {
                int num = std::stoi ( suffix );
                usedNumbers.push_back ( num );
                }
                catch (...)
                    {
                        // Not a number, ignore
                    }
            }
        }

        // Sort numbers
    std::sort ( usedNumbers.begin (), usedNumbers.end () );

    // Find first available number
    int nextNumber = baseNameExists ? 1 : 0;
    for (int num : usedNumbers)
        {
        if (num == nextNumber)
            {
            nextNumber++;
            }
        else if (num > nextNumber)
            {
            break; // Found a gap
            }
        }

        // If nextNumber is 0, return baseDisplayName (shouldn't happen here)
    if (nextNumber == 0)
        {
        return baseDisplayName;
        }

    return baseDisplayName + "_" + std::to_string ( nextNumber );
    }

bool CObject::RenameOwnedObject ( const std::string & oldDisplayName, const std::string & newDisplayName )
    {
    if (oldDisplayName.empty ())
        {
        std::cerr << "Error: Old display name cannot be empty!\n";
        return false;
        }

    if (newDisplayName.empty ())
        {
        std::cerr << "Error: New display name cannot be empty!\n";
        return false;
        }

        // Check if new display name is already used among children
    if (FindOwned ( newDisplayName ))
        {
        std::cerr << "Error: Display name '" << newDisplayName << "' already in use among children!\n";
        return false;
        }

    CObject * obj = FindOwned ( oldDisplayName );
    if (obj)
        {
        return obj->Rename ( newDisplayName );
        }

    std::cerr << "Error: Object '" << oldDisplayName << "' not found!\n";
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

bool CObject::Rename ( const std::string & newDisplayName )
    {
    if (newDisplayName.empty ())
        {
        std::cerr << "Error: New display name cannot be empty!\n";
        return false;
        }

        // If display name hasn't changed
    if (newDisplayName == DisplayName)
        {
        return true;
        }

        // Get hierarchy root
    CObject * root = GetRoot ();

    // Global uniqueness check
    if (root->FindRecursive ( newDisplayName ))
        {
        std::cout << "Error: can't Rename to '" << newDisplayName
            << "'. This display name already exists in world.\n";

        auto existingObj = FindObjectByDisplayNameRecursive ( newDisplayName );
        if (existingObj && typeid( *existingObj ) == typeid( *this ))
            {
            std::cout << "Objects have same type, generating unique display name...\n";
            std::cout << typeid( *existingObj ).name () << " found '"
                << existingObj->GetUniqName () << "' -> this '"
                << typeid( *this ).name () << "' " << this->GetUniqName () << "\n";

            // Generate unique display name
            std::string uniqueDisplayName = GenerateUniqueDisplayNameVariant ( newDisplayName, root );
            std::cout << "Auto-generating unique display name: '" << uniqueDisplayName << "'\n";

            // Apply new display name
            std::string oldDisplayName = DisplayName;
            DisplayName = uniqueDisplayName;
            std::cout << "Renamed '" << oldDisplayName << "' to '" << uniqueDisplayName
                << "' new unique name '" << GetUniqName () << "'\n";
            return true;
            }
        return false;
        }

        // Display name is unique, rename
    std::string oldDisplayName = DisplayName;
    DisplayName = newDisplayName;
    std::cout << "Renamed '" << oldDisplayName << "' to '" << newDisplayName
        << "' new unique name '" << GetUniqName () << "'\n";
    return true;
    }