#pragma once
#include <CoreMinimal.h>
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>

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

    // ownership
    void SetOwner(const CObject* NewOwner);
    void ClearOwner() { OwnerObject = nullptr; }
    const CObject* GetOwner() const { return OwnerObject; }

    // name
    void Rename(const std::string& NewName) { m_Name = NewName; }
    const std::string& GetName() const { return m_Name; }

    // manage owned objects
    // New API: caller passes ownership via unique_ptr; function returns raw pointer for convenience
    CObject* AddOwnedObject(std::unique_ptr<CObject> Obj);
    // convenience overload: take raw pointer and assume ownership
    CObject* AddOwnedObject(CObject* Obj) { return AddOwnedObject(std::unique_ptr<CObject>(Obj)); }

    void RemoveOwnedObject(CObject* Obj, bool bDeleteObject = false);
    void ClearOwnedObjects(bool bDeleteObjects = true);

    // find owned objects
    CObject* FindOwnedObject(const std::string& Name) const;
    CObject* FindOwnedObject(const CObject* Obj) const;

    // debugging info and hierarchy /* delete later */
    void PrintInfo() const;
    void PrintHierarchy(int Depth = 0) const;

    // counting owned objects
    size_t GetOwnedObjectsCount() const { return OwnedObjects.size(); }
    bool HasOwnedObjects() const { return !OwnedObjects.empty(); }

    // check ownership
    bool IsOwnerOf(const CObject* Obj) const;

protected:
    const CObject* OwnerObject = nullptr;
    std::string m_Name;
    std::vector<std::unique_ptr<CObject>> OwnedObjects;

    // registry of live objects to avoid dereferencing destroyed owners
    static std::unordered_set<const CObject*> s_AliveObjects;
};