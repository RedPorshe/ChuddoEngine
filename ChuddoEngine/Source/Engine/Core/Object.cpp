#include "Core/Object.h"
#include <iostream>
#include <algorithm>

std::unordered_set<const CObject*> CObject::s_AliveObjects;

CObject::CObject(const CObject* Owner, const std::string& inName)
    : OwnerObject(nullptr), m_Name(inName)
{
    s_AliveObjects.insert(this);

    std::cout << "Object '" << m_Name << "' (this=" << this << ") created";
    if (Owner)
    {
        if (s_AliveObjects.find(Owner) != s_AliveObjects.end())
        {
            // Record owner pointer but do NOT transfer ownership here. Ownership must be explicit via AddOwnedObject.
            OwnerObject = Owner;
            std::cout << " with owner '" << Owner->GetName() << "' (owner=" << Owner << ")." << std::endl;
        }
        else
        {
            std::cout << " with owner <dead object> (owner=" << Owner << ")." << std::endl;
            // Do not set owner to a dead object
            OwnerObject = nullptr;
        }
    }
    else
    {
        std::cout << " with no owner." << std::endl;
    }
}

CObject::CObject(const std::string& inName)
    : OwnerObject(nullptr), m_Name(inName)
{
    s_AliveObjects.insert(this);
    std::cout << "Object '" << m_Name << "' (this=" << this << ") created with no owner." << std::endl;
}

CObject::~CObject()
{
    std::cout << "~CObject: '" << GetName() << "' (this=" << this << ") starting destruction." << std::endl;

    // remove from alive registry first to prevent others from dereferencing us while we clean up
    s_AliveObjects.erase(this);

    // Safely clear owned objects first so their destructors do not
    // try to access this parent while it's in the process of being destroyed.
    if (HasOwnedObjects())
    {
        ClearOwnedObjects(true);
    }

    // Then notify owner that this object is gone, only if owner is still alive
    if (OwnerObject && s_AliveObjects.find(OwnerObject) != s_AliveObjects.end())
    {
        std::cout << "~CObject: notifying owner (owner=" << OwnerObject << ") to remove this." << std::endl;
        const_cast<CObject*>(OwnerObject)->RemoveOwnedObject(this, false);
    }

    std::cout << "Object '" << GetName() << "' (this=" << this << ") destroyed." << std::endl;
}

void CObject::SetOwner(const CObject* NewOwner)
{
    std::cout << "SetOwner: object=" << this << " newOwner=" << NewOwner << std::endl;

    if (OwnerObject == NewOwner)
        return;

    if (NewOwner == this)
    {
        std::cerr << "Warning: An object cannot own itself. Owner not changed." << std::endl;
        return;
    }

    if (NewOwner && s_AliveObjects.find(NewOwner) == s_AliveObjects.end())
    {
        std::cerr << "Warning: Attempt to set owner to dead object. Owner not changed." << std::endl;
        return;
    }

    if (NewOwner && const_cast<CObject*>(NewOwner)->IsOwnerOf(this))
    {
        std::cerr << "Error: Circular ownership detected. Owner not changed." << std::endl;
        return;
    }

    // Do not implicitly remove/add ownership from OwnedObjects here; ownership must be managed explicitly
    OwnerObject = NewOwner;
}

CObject* CObject::AddOwnedObject(std::unique_ptr<CObject> Obj)
{
    CObject* raw = Obj.get();
    std::cout << "AddOwnedObject: owner=" << this << " obj=" << raw << std::endl;

    if (!raw || raw == this)
        return nullptr;

    if (s_AliveObjects.find(raw) == s_AliveObjects.end())
    {
        std::cerr << "AddOwnedObject: attempt to add dead object (" << raw << ")" << std::endl;
        return nullptr;
    }

    // check duplicate
    auto itExisting = std::find_if(OwnedObjects.begin(), OwnedObjects.end(),
        [raw](const std::unique_ptr<CObject>& Up) { return Up.get() == raw; });
    if (itExisting != OwnedObjects.end())
        return raw;

    // circular ownership
    if (raw->IsOwnerOf(this))
    {
        std::cerr << "Error: Circular ownership detected!" << std::endl;
        return nullptr;
    }

    // detach from previous owner if any
    if (raw->OwnerObject && raw->OwnerObject != this)
    {
        if (s_AliveObjects.find(raw->OwnerObject) != s_AliveObjects.end())
            const_cast<CObject*>(raw->OwnerObject)->RemoveOwnedObject(raw, false);
    }

    // take ownership
    OwnedObjects.emplace_back(std::move(Obj));
    raw->OwnerObject = this;
    return raw;
}

void CObject::RemoveOwnedObject(CObject* Obj, bool bDeleteObject)
{
    std::cout << "RemoveOwnedObject: owner=" << this << " obj=" << Obj << " delete=" << bDeleteObject << std::endl;

    auto it = std::find_if(OwnedObjects.begin(), OwnedObjects.end(),
        [Obj](const std::unique_ptr<CObject>& Up) { return Up.get() == Obj; });
    if (it != OwnedObjects.end())
    {
        CObject* raw = it->get();
        if (raw->OwnerObject == this)
        {
            raw->OwnerObject = nullptr;
        }

        if (!bDeleteObject)
        {
            // release ownership without deleting
            it->release();
            OwnedObjects.erase(it);
        }
        else
        {
            // erase will destroy unique_ptr and delete the object
            OwnedObjects.erase(it);
        }
    }
    else
    {
        std::cerr << "RemoveOwnedObject: object not found among owned objects." << std::endl;
    }
}

void CObject::ClearOwnedObjects(bool bDeleteObjects)
{
    std::cout << "ClearOwnedObjects: owner=" << this << " delete=" << bDeleteObjects << " count=" << OwnedObjects.size() << std::endl;

    // move ownership out so nested destructors cannot modify OwnedObjects
    std::vector<std::unique_ptr<CObject>> Temp = std::move(OwnedObjects);
    OwnedObjects.clear();

    if (bDeleteObjects)
    {
        for (auto& Up : Temp)
        {
            if (Up)
            {
                std::cout << " ClearOwnedObjects: destroying child=" << Up.get() << std::endl;
                Up->OwnerObject = nullptr;
                // unique_ptr will delete when Temp is destroyed
            }
        }
        // Temp goes out of scope and deletes owned objects
    }
    else
    {
        for (auto& Up : Temp)
        {
            if (Up)
            {
                std::cout << " ClearOwnedObjects: releasing child=" << Up.get() << std::endl;
                Up->OwnerObject = nullptr;
                Up.release(); // release ownership, do not delete
            }
        }
        // Temp destructs, but released pointers remain allocated
    }
}


CObject* CObject::FindOwnedObject(const std::string& Name) const
{
    auto it = std::find_if(OwnedObjects.begin(), OwnedObjects.end(),
        [&Name](const std::unique_ptr<CObject>& Up) {
            return Up && Up->GetName() == Name;
        });

    return (it != OwnedObjects.end()) ? it->get() : nullptr;
}

CObject* CObject::FindOwnedObject(const CObject* Obj) const
{
    auto it = std::find_if(OwnedObjects.begin(), OwnedObjects.end(),
        [Obj](const std::unique_ptr<CObject>& Up) { return Up.get() == Obj; });
    return (it != OwnedObjects.end()) ? it->get() : nullptr;
}

bool CObject::IsOwnerOf(const CObject* Obj) const
{
    while (Obj)
    {
        if (Obj == this) return true;
        Obj = Obj->OwnerObject;
    }
    return false;
}

void CObject::SetName(const std::string& NewName)
{
    m_Name = NewName;
}

void CObject::PrintInfo() const
{
    std::cout << "This is object named '" << m_Name << "'. ";
    if (OwnerObject && s_AliveObjects.find(OwnerObject) != s_AliveObjects.end())
    {
        std::cout << "It is owned by '" << OwnerObject->GetName() << "'.";
    }
    else
    {
        std::cout << "It has no owner.";
    }
    std::cout << " Owns " << OwnedObjects.size() << " object(s)." << std::endl;
}

void CObject::PrintHierarchy(int Depth) const
{
    std::string indent(Depth * 2, ' ');
    std::cout << indent << "- " << m_Name;
    if (OwnerObject && s_AliveObjects.find(OwnerObject) != s_AliveObjects.end())
        std::cout << " [Owner: " << OwnerObject->GetName() << "]";
    std::cout << std::endl;

    for (const auto& childUp : OwnedObjects)
    {
        if (childUp)
        {
            childUp->PrintHierarchy(Depth + 1);
        }
    }
}