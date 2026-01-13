#include "Core/Object.h"
#include <iostream>
#include <algorithm>

CObject::CObject(const CObject* Owner, const std::string& inName)
    : OwnerObject(Owner), m_Name(inName)
{
    std::cout << "Object '" << m_Name << "' created";
    if (Owner)
    {
        std::cout << " with owner '" << Owner->GetName() << "'." << std::endl;
        // Добавляем себя в список owned объектов родителя
        const_cast<CObject*>(Owner)->AddOwnedObject(this);
    }
    else
    {
        std::cout << " with no owner." << std::endl;
    }
}

CObject::CObject(const std::string& inName)
    : m_Name(inName)
{
    std::cout << "Object '" << m_Name << "' created with no owner." << std::endl;
}

CObject::~CObject()
{
    std::cout << "Object '" << m_Name << "' destroyed." << std::endl;

    // Уведомляем владельца о нашем уничтожении
    if (OwnerObject)
    {
        const_cast<CObject*>(OwnerObject)->RemoveOwnedObject(this, false);
    }

    // Удаляем все дочерние объекты
    ClearOwnedObjects(true);
}

void CObject::SetOwner(const CObject* NewOwner)
{
    if (OwnerObject == NewOwner)
        return;

    if (NewOwner == this)
    {
        std::cerr << "Warning: An object cannot own itself. Owner not changed." << std::endl;
        return;
    }

    // Проверка циклических зависимостей
    if (NewOwner && const_cast<CObject*>(NewOwner)->IsOwnerOf(this))
    {
        std::cerr << "Error: Circular ownership detected. Owner not changed." << std::endl;
        return;
    }

    // Удаляем себя из списка старого владельца
    if (OwnerObject)
    {
        const_cast<CObject*>(OwnerObject)->RemoveOwnedObject(this, false);
    }

    // Устанавливаем нового владельца
    OwnerObject = NewOwner;

    // Добавляем себя в список нового владельца
    if (OwnerObject)
    {
        const_cast<CObject*>(OwnerObject)->AddOwnedObject(this);
    }
}

void CObject::AddOwnedObject(CObject* Obj)
{
    if (!Obj || Obj == this)
        return;

    // Проверяем, не добавляем ли мы уже существующий объект
    if (std::find(OwnedObjects.begin(), OwnedObjects.end(), Obj) != OwnedObjects.end())
        return;

    // Проверяем циклические зависимости
    if (Obj->IsOwnerOf(this))
    {
        std::cerr << "Error: Circular ownership detected!" << std::endl;
        return;
    }

    // Если у объекта уже есть другой владелец
    if (Obj->OwnerObject && Obj->OwnerObject != this)
    {
        const_cast<CObject*>(Obj->OwnerObject)->RemoveOwnedObject(Obj, false);
    }

    OwnedObjects.push_back(Obj);
    Obj->OwnerObject = this;
}

void CObject::RemoveOwnedObject(CObject* Obj, bool bDeleteObject)
{
    auto it = std::find(OwnedObjects.begin(), OwnedObjects.end(), Obj);
    if (it != OwnedObjects.end())
    {
        OwnedObjects.erase(it);
        if (Obj->OwnerObject == this)
        {
            Obj->OwnerObject = nullptr;
        }

        if (bDeleteObject)
        {
            delete Obj;
        }
    }
}

void CObject::ClearOwnedObjects(bool bDeleteObjects)
{
    if (bDeleteObjects)
    {
        for (CObject* Obj : OwnedObjects)
        {
            if (Obj)
            {
                Obj->OwnerObject = nullptr;
                delete Obj;
            }
        }
    }
    else
    {
        for (CObject* Obj : OwnedObjects)
        {
            if (Obj)
            {
                Obj->OwnerObject = nullptr;
            }
        }
    }
    OwnedObjects.clear();
}

CObject* CObject::FindOwnedObject(const std::string& Name) const
{
    auto it = std::find_if(OwnedObjects.begin(), OwnedObjects.end(),
        [&Name](const CObject* Obj) {
            return Obj && Obj->GetName() == Name;
        });

    return (it != OwnedObjects.end()) ? *it : nullptr;
}

CObject* CObject::FindOwnedObject(const CObject* Obj) const
{
    auto it = std::find(OwnedObjects.begin(), OwnedObjects.end(), Obj);
    return (it != OwnedObjects.end()) ? *it : nullptr;
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

void CObject::PrintInfo() const
{
    std::cout << "This is object named '" << m_Name << "'. "
        << "It " << (OwnerObject ? "is owned by '" + OwnerObject->GetName() + "'."
            : "has no owner.")
        << " Owns " << OwnedObjects.size() << " object(s)." << std::endl;
}

void CObject::PrintHierarchy(int Depth) const
{
    std::string indent(Depth * 2, ' ');
    std::cout << indent << "- " << m_Name;
    if (OwnerObject)
        std::cout << " [Owner: " << OwnerObject->GetName() << "]";
    std::cout << std::endl;

    for (const CObject* child : OwnedObjects)
    {
        if (child)
        {
            child->PrintHierarchy(Depth + 1);
        }
    }
}