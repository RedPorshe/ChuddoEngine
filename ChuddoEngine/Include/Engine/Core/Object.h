#pragma once
#include <CoreMinimal.h>
#include <vector>
#include <string>
#include <memory>

class CObject
{
public:
    CObject(const CObject* Owner = nullptr, const std::string& inName = "Object");
    explicit CObject(const std::string& inName);

    virtual ~CObject();

    CObject(const CObject&) = delete;
    CObject& operator=(const CObject&) = delete;

    CObject(CObject&&) = default;
    CObject& operator=(CObject&&) = default;

    // Владение
    void SetOwner(const CObject* NewOwner);
    void ClearOwner() { OwnerObject = nullptr; }
    const CObject* GetOwner() const { return OwnerObject; }

    // Имя
    void Rename(const std::string& NewName) { m_Name = NewName; }
    const std::string& GetName() const { return m_Name; }

    // Управление дочерними объектами
    void AddOwnedObject(CObject* Obj);
    void RemoveOwnedObject(CObject* Obj, bool bDeleteObject = false);
    void ClearOwnedObjects(bool bDeleteObjects = true);

    // Поиск
    CObject* FindOwnedObject(const std::string& Name) const;
    CObject* FindOwnedObject(const CObject* Obj) const;

    // Информация
    void PrintInfo() const;
    void PrintHierarchy(int Depth = 0) const;

    // Количество
    size_t GetOwnedObjectsCount() const { return OwnedObjects.size(); }
    bool HasOwnedObjects() const { return !OwnedObjects.empty(); }

    // Проверка владения
    bool IsOwnerOf(const CObject* Obj) const;

protected:
    const CObject* OwnerObject = nullptr;
    std::string m_Name;
    std::vector<CObject*> OwnedObjects;
};