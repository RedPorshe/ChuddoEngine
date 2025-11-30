#include "Engine/Core/Config.h"

#include <algorithm>


  Config& Config::Get()
  {
    static Config instance;
    return instance;
  }

  bool Config::Load(const std::string& filename)
  {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
      CORE_WARN("Config file not found, creating default: ", filename);
      SetupDefaults();
      return Save(filename);
    }

    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string encryptedData = buffer.str();

    if (encryptedData.empty())
    {
      CORE_ERROR("Config file is empty: ", filename);
      return false;
    }

    
    std::string decryptedData = Decrypt(encryptedData);
    std::stringstream decryptedStream(decryptedData);

    m_Data.clear();
    std::string line;
    while (std::getline(decryptedStream, line))
    {
      size_t delimiter = line.find('=');
      if (delimiter != std::string::npos)
      {
        std::string key = line.substr(0, delimiter);
        std::string value = line.substr(delimiter + 1);
        m_Data[key] = value;
      }
    }

    CORE_DEBUG("Config loaded from: ", filename, " (", m_Data.size(), " entries)");
    return true;
  }

  bool Config::Save(const std::string& filename)
  {
    std::stringstream data;
    for (const auto& [key, value] : m_Data)
    {
      data << key << "=" << value << "\n";
    }

    
    std::string encryptedData = Encrypt(data.str());

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
      CORE_ERROR("Failed to save config: ", filename);
      return false;
    }

    file << encryptedData;
    file.close();

    CORE_DEBUG("Config saved to: ", filename, " (", m_Data.size(), " entries)");
    return true;
  }

  void Config::SetupDefaults()
  {
    
    SetInt("WindowWidth", 1024);
    SetInt("WindowHeight", 768);
    SetBool("Fullscreen", false);
    SetBool("VSync", true);
    SetInt("MSAASamples", 4);
    SetInt("MaxFPS", 120);

    
    SetInt("TextureQuality", 2);  
    SetInt("ShadowQuality", 2);
    SetBool("PostProcessing", true);
    SetBool("Bloom", true);

    
    SetFloat("MasterVolume", 1.0f);
    SetFloat("MusicVolume", 0.8f);
    SetFloat("SFXVolume", 1.0f);

    
    SetFloat("MouseSensitivity", 1.0f);
    SetBool("InvertMouseY", false);

    
    SetBool("ShowFPS", true);
    SetBool("EnableDebugDraw", false);
    SetInt("LogLevel", 2);  // 0=Error, 1=Warning, 2=Info, 3=Debug, 4=Verbose

    CORE_DEBUG("Default config setup complete");
  }

 
  std::string Config::GetString(const std::string& key, const std::string& defaultValue)
  {
    auto it = m_Data.find(key);
    return it != m_Data.end() ? it->second : defaultValue;
  }

  int Config::GetInt(const std::string& key, int defaultValue)
  {
    auto it = m_Data.find(key);
    if (it != m_Data.end())
    {
      try
      {
        return std::stoi(it->second);
      }
      catch (...)
      {
        CORE_WARN("Invalid int value for key: ", key);
      }
    }
    return defaultValue;
  }

  float Config::GetFloat(const std::string& key, float defaultValue)
  {
    auto it = m_Data.find(key);
    if (it != m_Data.end())
    {
      try
      {
        return std::stof(it->second);
      }
      catch (...)
      {
        CORE_WARN("Invalid float value for key: ", key);
      }
    }
    return defaultValue;
  }

  bool Config::GetBool(const std::string& key, bool defaultValue)
  {
    auto it = m_Data.find(key);
    if (it != m_Data.end())
    {
      std::string value = it->second;
      std::transform(value.begin(), value.end(), value.begin(), ::tolower);
      return (value == "true" || value == "1" || value == "yes");
    }
    return defaultValue;
  }

  
  void Config::SetString(const std::string& key, const std::string& value)
  {
    m_Data[key] = value;
  }

  void Config::SetInt(const std::string& key, int value)
  {
    m_Data[key] = std::to_string(value);
  }

  void Config::SetFloat(const std::string& key, float value)
  {
    m_Data[key] = std::to_string(value);
  }

  void Config::SetBool(const std::string& key, bool value)
  {
    m_Data[key] = value ? "true" : "false";
  }

  
  std::vector<std::string> Config::GetAllKeys() const
  {
    std::vector<std::string> keys;
    keys.reserve(m_Data.size());

    for (const auto& [key, value] : m_Data)
    {
      keys.push_back(key);
    }

    std::sort(keys.begin(), keys.end());
    return keys;
  }

  bool Config::HasKey(const std::string& key) const
  {
    return m_Data.find(key) != m_Data.end();
  }

  void Config::RemoveKey(const std::string& key)
  {
    auto it = m_Data.find(key);
    if (it != m_Data.end())
    {
      m_Data.erase(it);
      CORE_DEBUG("Removed config key: ", key);
    }
  }

  
  std::string Config::Encrypt(const std::string& data) const
  {
    const std::string key = "ChuddoEngineConfig2024";
    std::string result = data;

    for (size_t i = 0; i < data.size(); ++i)
    {
      result[i] = data[i] ^ key[i % key.size()];
    }
    return result;
  }

  std::string Config::Decrypt(const std::string& data) const
  {
    return Encrypt(data);  
  }