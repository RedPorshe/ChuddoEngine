#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "CoreMinimal.h"

namespace CE
{
  class CEComponent;

  class CEObject
  {
   public:
    CEObject(CEObject* Owner = nullptr, FString NewName = "Object");
    virtual ~CEObject();

    virtual void BeginPlay();
    virtual void Update(float DeltaTime);
    virtual void Tick(float DeltaTime);

    bool HasOwner() const
    {
      return m_Owner != nullptr;
    }

    template <typename T, typename... Args>
    T* AddSubObject(const std::string& Name, Args&&... args);

    CEObject* GetOwner() const;

    void SetOwner(CEObject* Owner);
    const FString& GetName() const
    {
      return m_Name;
    }

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

   private:
    template <typename T>
    T* GetComponent(const std::string& Name);

   protected:
    std::unordered_map<std::string, std::unique_ptr<CEComponent>> m_Components;
    CEObject* m_Owner = nullptr;
    FString m_Name{};
  };

  template <typename T, typename... Args>
  T* CEObject::AddSubObject(const std::string& Name, Args&&... args)
  {
    auto component = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = component.get();
    m_Components[Name] = std::move(component);
    return ptr;
  }

  template <typename T>
  T* CEObject::GetComponent(const std::string& Name)
  {
    auto it = m_Components.find(Name);
    if (it != m_Components.end())
    {
      return dynamic_cast<T*>(it->second.get());
    }
    return nullptr;
  }
}  // namespace CE