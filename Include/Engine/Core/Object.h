// CEObject.h
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "CoreMinimal.h"
#include "Engine/Core/Reflection.h"

namespace CE
{
  class CComponent;

  class CObject
  {
   public:
    CObject(CObject* Owner = nullptr, FString NewName = "Object");
    virtual ~CObject();

    virtual void BeginPlay();
    virtual void Update(float DeltaTime);
    virtual void Tick(float DeltaTime);

    bool HasOwner() const
    {
      return m_Owner != nullptr;
    }

    template <typename T, typename... Args>
    T* AddSubObject(const std::string& Name, Args&&... args);

    CObject* GetOwner() const;

    void SetOwner(CObject* Owner);
    const FString& GetName() const
    {
      return m_Name;
    }

    virtual const ClassInfo* GetClassInfo() const { return nullptr; }

    template <typename T>
    std::vector<T*> GetComponents() const
    {
      std::vector<T*> result;
      for (const auto& [name, component] : m_Components)
      {
        if (auto* casted = dynamic_cast<T*>(component.get()))
        {
          result.push_back(casted);
        }
      }
      return result;
    }

    // НОВАЯ ФУНКЦИЯ: Итерация по всем компонентам определенного типа
    template <typename T>
    void ForEachComponent(std::function<void(T*)> callback) const
    {
      for (const auto& [name, component] : m_Components)
      {
        if (auto* casted = dynamic_cast<T*>(component.get()))
        {
          callback(casted);
        }
      }
    }

    // НОВАЯ ФУНКЦИЯ: Итерация по всем компонентам (без типа)
    void ForEachComponent(std::function<void(CComponent*)> callback) const
    {
      for (const auto& [name, component] : m_Components)
      {
        callback(component.get());
      }
    }

   
    template <typename T>
    T* GetComponent(const std::string& Name);

   protected:
    std::unordered_map<std::string, std::unique_ptr<CComponent>> m_Components;
    CObject* m_Owner = nullptr;
    FString m_Name{};
  };

  template <typename T, typename... Args>
  T* CObject::AddSubObject(const std::string& Name, Args&&... args)
  {
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = component.get();
    m_Components[Name] = std::move(component);
    return ptr;
  }

  template <typename T>
  T* CObject::GetComponent(const std::string& Name)
  {
    auto it = m_Components.find(Name);
    if (it != m_Components.end())
    {
      return dynamic_cast<T*>(it->second.get());
    }
    return nullptr;
  }
}  // namespace CE