#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "CoreMinimal.h"


  class Config
  {
   public:
    static Config& Get();

    bool Load(const std::string& filename = "engine.cfg");
    bool Save(const std::string& filename = "engine.cfg");

    
    std::string GetString(const std::string& key, const std::string& defaultValue = "");
    int GetInt(const std::string& key, int defaultValue = 0);
    float GetFloat(const std::string& key, float defaultValue = 0.0f);
    bool GetBool(const std::string& key, bool defaultValue = false);

    
    void SetString(const std::string& key, const std::string& value);
    void SetInt(const std::string& key, int value);
    void SetFloat(const std::string& key, float value);
    void SetBool(const std::string& key, bool value);

    
    std::vector<std::string> GetAllKeys() const;
    bool HasKey(const std::string& key) const;
    void RemoveKey(const std::string& key);

    
    void SetupDefaults();

   private:
    Config() = default;
    ~Config() = default;

    std::unordered_map<std::string, std::string> m_Data;

    
    std::string Encrypt(const std::string& data) const;
    std::string Decrypt(const std::string& data) const;
  };