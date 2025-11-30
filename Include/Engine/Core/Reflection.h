#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include <typeinfo>
#include <typeindex>

#include "CoreMinimal.h"
#include "Engine/Utils/Math/AllMath.h"

namespace CE
{
  // Forward declarations
  class CObject;
  class Property;
  class Function;

  // Property types
  enum class EPropertyType
  {
    Bool,
    Int,
    Float,
    Double,
    String,
    Vector2,
    Vector3,
    Vector4,
    Quaternion,
    Color,
    Custom
  };

  // Property flags
  enum class EPropertyFlags
  {
    None = 0,
    EditAnywhere = 1 << 0,
    EditInstanceOnly = 1 << 1,
    VisibleAnywhere = 1 << 2,
    ReadOnly = 1 << 3,
    Transient = 1 << 4
  };

  inline EPropertyFlags operator|(EPropertyFlags a, EPropertyFlags b)
  {
    return static_cast<EPropertyFlags>(static_cast<int>(a) | static_cast<int>(b));
  }

  inline bool operator&(EPropertyFlags a, EPropertyFlags b)
  {
    return static_cast<int>(a) & static_cast<int>(b);
  }

  // Property metadata
  struct PropertyMeta
  {
    std::string DisplayName;
    std::string Category = "Default";
    std::string Description;
    EPropertyFlags Flags = EPropertyFlags::EditAnywhere;
    float MinValue = 0.0f;
    float MaxValue = 0.0f;
    std::string CustomEditor;

    PropertyMeta() = default;
    PropertyMeta(const std::string& displayName, const std::string& category = "Default",
                 const std::string& description = "", EPropertyFlags flags = EPropertyFlags::EditAnywhere,
                 float minVal = 0.0f, float maxVal = 0.0f, const std::string& customEditor = "")
        : DisplayName(displayName), Category(category), Description(description), Flags(flags),
          MinValue(minVal), MaxValue(maxVal), CustomEditor(customEditor) {}
  };

  // Base property class
  class Property
  {
   public:
    Property(const std::string& name, EPropertyType type, const PropertyMeta& meta = PropertyMeta())
        : m_Name(name), m_Type(type), m_Meta(meta)
    {
    }
    virtual ~Property() = default;

    const std::string& GetName() const { return m_Name; }
    EPropertyType GetType() const { return m_Type; }
    const PropertyMeta& GetMeta() const { return m_Meta; }

    virtual void* GetValuePtr(void* object) const = 0;
    virtual void SetValueFromString(void* object, const std::string& value) = 0;
    virtual std::string GetValueAsString(void* object) const = 0;

   protected:
    std::string m_Name;
    EPropertyType m_Type;
    PropertyMeta m_Meta;
  };

  // Template property class
  template <typename T>
  class TProperty : public Property
  {
   public:
    TProperty(const std::string& name, std::function<T&(void*)> getter, std::function<void(void*, const T&)> setter, const PropertyMeta& meta = PropertyMeta())
        : Property(name, GetPropertyType<T>(), meta), m_Getter(getter), m_Setter(setter)
    {
    }

    void* GetValuePtr(void* object) const override
    {
      return &m_Getter(object);
    }

    void SetValueFromString(void* object, const std::string& value) override
    {
      T val = StringToValue<T>(value);
      m_Setter(object, val);
    }

    std::string GetValueAsString(void* object) const override
    {
      return ValueToString(m_Getter(object));
    }

   private:
    std::function<T&(void*)> m_Getter;
    std::function<void(void*, const T&)> m_Setter;

    static EPropertyType GetPropertyType()
    {
      if constexpr (std::is_same_v<T, bool>) return EPropertyType::Bool;
      else if constexpr (std::is_same_v<T, int>) return EPropertyType::Int;
      else if constexpr (std::is_same_v<T, float>) return EPropertyType::Float;
      else if constexpr (std::is_same_v<T, double>) return EPropertyType::Double;
      else if constexpr (std::is_same_v<T, FString>) return EPropertyType::String;
      else if constexpr (std::is_same_v<T, CE::Math::Vector2f>) return EPropertyType::Vector2;
      else if constexpr (std::is_same_v<T, CE::Math::Vector3f>) return EPropertyType::Vector3;
      else if constexpr (std::is_same_v<T, CE::Math::Vector4f>) return EPropertyType::Vector4;
      else if constexpr (std::is_same_v<T, CE::Math::Quaternionf>) return EPropertyType::Quaternion;
      else if constexpr (std::is_same_v<T, CE::Math::Color>) return EPropertyType::Color;
      else return EPropertyType::Custom;
    }

    static T StringToValue(const std::string& str)
    {
      if constexpr (std::is_same_v<T, bool>) return str == "true" || str == "1";
      else if constexpr (std::is_same_v<T, int>) return std::stoi(str);
      else if constexpr (std::is_same_v<T, float>) return std::stof(str);
      else if constexpr (std::is_same_v<T, double>) return std::stod(str);
      else if constexpr (std::is_same_v<T, FString>) return FString(str.c_str());
      else if constexpr (std::is_same_v<T, CE::Math::Vector2f>)
      {
        CE::Math::Vector2f v;
        sscanf(str.c_str(), "%f,%f", &v.x, &v.y);
        return v;
      }
      else if constexpr (std::is_same_v<T, CE::Math::Vector3f>)
      {
        CE::Math::Vector3f v;
        sscanf(str.c_str(), "%f,%f,%f", &v.x, &v.y, &v.z);
        return v;
      }
      else if constexpr (std::is_same_v<T, CE::Math::Vector4f>)
      {
        CE::Math::Vector4f v;
        sscanf(str.c_str(), "%f,%f,%f,%f", &v.x, &v.y, &v.z, &v.w);
        return v;
      }
      else if constexpr (std::is_same_v<T, CE::Math::Quaternionf>)
      {
        CE::Math::Quaternionf q;
        sscanf(str.c_str(), "%f,%f,%f,%f", &q.x, &q.y, &q.z, &q.w);
        return q;
      }
      else if constexpr (std::is_same_v<T, CE::Math::Color>)
      {
        CE::Math::Color c;
        sscanf(str.c_str(), "%f,%f,%f,%f", &c.x, &c.y, &c.z, &c.w);
        return c;
      }
      else
      {
        // For custom types, return default
        return T{};
      }
    }

    static std::string ValueToString(const T& val)
    {
      if constexpr (std::is_same_v<T, bool>) return val ? "true" : "false";
      else if constexpr (std::is_same_v<T, int>) return std::to_string(val);
      else if constexpr (std::is_same_v<T, float>) return std::to_string(val);
      else if constexpr (std::is_same_v<T, double>) return std::to_string(val);
      else if constexpr (std::is_same_v<T, FString>) return std::string(val.c_str());
      else if constexpr (std::is_same_v<T, Math::Vector2f>) return std::to_string(val.x) + "," + std::to_string(val.y);
      else if constexpr (std::is_same_v<T, Math::Vector3f>) return std::to_string(val.x) + "," + std::to_string(val.y) + "," + std::to_string(val.z);
      else if constexpr (std::is_same_v<T, Math::Vector4f>) return std::to_string(val.x) + "," + std::to_string(val.y) + "," + std::to_string(val.z) + "," + std::to_string(val.w);
      else if constexpr (std::is_same_v<T, Math::Quaternionf>) return std::to_string(val.x) + "," + std::to_string(val.y) + "," + std::to_string(val.z) + "," + std::to_string(val.w);
      else if constexpr (std::is_same_v<T, Math::Color>) return std::to_string(val.GetR()) + "," + std::to_string(val.GetG()) + "," + std::to_string(val.GetB()) + "," + std::to_string(val.GetA());
      else return "custom";
    }
  };

  // Function class for UFUNCTION
  class Function
  {
   public:
    Function(const std::string& name, std::function<void(void*)> func, const std::string& category = "Default")
        : m_Name(name), m_Function(func), m_Category(category)
    {
    }

    const std::string& GetName() const { return m_Name; }
    const std::string& GetCategory() const { return m_Category; }
    void Invoke(void* object) { m_Function(object); }

   private:
    std::string m_Name;
    std::function<void(void*)> m_Function;
    std::string m_Category;
  };

  // Class info for UCLASS
  class ClassInfo
  {
   public:
    ClassInfo(const std::string& name, std::function<CObject*()> factory = nullptr)
        : m_Name(name), m_Factory(factory)
    {
    }

    const std::string& GetName() const { return m_Name; }
    CObject* CreateInstance() const { return m_Factory ? m_Factory() : nullptr; }

    void AddProperty(std::unique_ptr<Property> prop)
    {
      m_Properties.push_back(std::move(prop));
    }

    void AddFunction(std::unique_ptr<Function> func)
    {
      m_Functions.push_back(std::move(func));
    }

    const std::vector<std::unique_ptr<Property>>& GetProperties() const { return m_Properties; }
    const std::vector<std::unique_ptr<Function>>& GetFunctions() const { return m_Functions; }

   private:
    std::string m_Name;
    std::function<CObject*()> m_Factory;
    std::vector<std::unique_ptr<Property>> m_Properties;
    std::vector<std::unique_ptr<Function>> m_Functions;
  };

  // Reflection registry
  class ReflectionRegistry
  {
   public:
    static ReflectionRegistry& Get()
    {
      static ReflectionRegistry instance;
      return instance;
    }

    void RegisterClass(std::unique_ptr<ClassInfo> classInfo)
    {
      m_Classes[classInfo->GetName()] = std::move(classInfo);
    }

    const ClassInfo* GetClassInfo(const std::string& name) const
    {
      auto it = m_Classes.find(name);
      return it != m_Classes.end() ? it->second.get() : nullptr;
    }

    const std::unordered_map<std::string, std::unique_ptr<ClassInfo>>& GetAllClasses() const
    {
      return m_Classes;
    }

   private:
    std::unordered_map<std::string, std::unique_ptr<ClassInfo>> m_Classes;
  };

  // Macros
#define CCLASS(ClassName) \
  static ClassInfo* ClassName##_ClassInfo = nullptr; \
  static void Register##ClassName() { \
    if (!ClassName##_ClassInfo) { \
      ClassName##_ClassInfo = new ClassInfo(#ClassName); \
      ReflectionRegistry::Get().RegisterClass(std::unique_ptr<ClassInfo>(ClassName##_ClassInfo)); \
    } \
  } \
  static int ClassName##_Dummy = (Register##ClassName(), 0);

#define CPROPERTY(Class, Type, Name, ...) \
  Type Name; \
  static int Name##_Dummy = (Class##_ClassInfo->AddProperty(std::make_unique<TProperty<Type>>(#Name, \
    [](void* obj) -> Type& { return static_cast<Class*>(obj)->Name; }, \
    [](void* obj, const Type& val) { static_cast<Class*>(obj)->Name = val; }, \
    PropertyMeta{__VA_ARGS__})), 0)

#define CFUNCTION(Name, Category) \
  void Name(); \
  static int Name##_Dummy = (ClassName##_ClassInfo->AddFunction(std::make_unique<Function>(#Name, \
    [](void* obj) { static_cast<ClassName*>(obj)->Name(); }, Category)), 0)

}  // namespace CE
