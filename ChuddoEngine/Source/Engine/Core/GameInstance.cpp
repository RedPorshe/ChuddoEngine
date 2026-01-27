#include "Core/GameInstance.h"
#include "Core/World.h"
#include "Core/Level.h"

#include <memory>
#include <fstream> 
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

std::unique_ptr<CGameInstance> CGameInstance::Instance = nullptr;
bool CGameInstance::bInstanceCreated = false;
std::string CGameInstance::m_ConfigFilePath = "GameInstanceConfig.ini";
std::string CGameInstance::MyKey = "Chuddo";

void CGameInstance::DefaultInitialization()
{
	auto WorldUP = std::make_unique<CWorld>(this, "default World");
	auto word = static_cast<CWorld*>(this->AddOwnedObject(std::move(WorldUP)));
	word->LoadLevel ( "Default init level" );
	word->SetCurrentLevel ( word->GetPersistentLevel () );
	SetWorld(word);
}

CGameInstance::CGameInstance(const std::string& inName)
	: CObject(inName)
{

}

CGameInstance::CGameInstance(const CObject* Owner, const std::string& inName)
	: CObject(Owner, inName)
{

}

CGameInstance::~CGameInstance()
{

}

bool CGameInstance::LoadFromConfig()
{
	try
	{
		std::cout << "Loading encrypted configuration from: " << m_ConfigFilePath << std::endl;

		// Читаем зашифрованный файл как бинарный
		std::ifstream configFile(m_ConfigFilePath, std::ios::binary);
		if (!configFile.is_open())
		{
			std::cerr << "ERROR: Cannot open encrypted config file: "
				<< m_ConfigFilePath << std::endl;
			return false;
		}

		// Читаем все данные
		std::string encryptedData(
			(std::istreambuf_iterator<char>(configFile)),
			std::istreambuf_iterator<char>()
		);
		configFile.close();

		// Дешифруем
		std::string plainText = Encript(encryptedData, MyKey);

		// Теперь парсим обычный текст
		std::stringstream ss(plainText);
		std::string line;
		std::string currentSection;

		std::string gameInstanceName;
		std::string worldName;
		bool hasWorld = false;

		while (std::getline(ss, line))
		{
			// Пропускаем комментарии и пустые строки
			if (line.empty() || line[0] == '#')
				continue;

			// Обработка секций
			if (line[0] == '[' && line.back() == ']')
			{
				currentSection = line.substr(1, line.length() - 2);
				continue;
			}

			// Парсинг ключ=значение
			size_t equalsPos = line.find('=');
			if (equalsPos != std::string::npos)
			{
				std::string key = line.substr(0, equalsPos);
				std::string value = line.substr(equalsPos + 1);

				// Убираем пробелы
				key.erase(0, key.find_first_not_of(" \t"));
				key.erase(key.find_last_not_of(" \t") + 1);
				value.erase(0, value.find_first_not_of(" \t"));
				value.erase(value.find_last_not_of(" \t") + 1);

				if (currentSection == "GameInstance")
				{
					if (key == "Name")
					{
						gameInstanceName = value;
					}
				}
				else if (currentSection == "World")
				{
					if (key == "Name")
					{
						worldName = value;
					}
					else if (key == "Exists")
					{
						hasWorld = (value == "true" || value == "True" || value == "TRUE");
					}
				}
			}
		}

		std::cout << "Decrypted config loaded:" << std::endl;
		std::cout << "  GameInstance Name: " << gameInstanceName << std::endl;
		std::cout << "  Has World: " << (hasWorld ? "true" : "false") << std::endl;
		std::cout << "  World Name: " << worldName << std::endl;

		// Создаем World на основе загруженных данных
		if (hasWorld && !worldName.empty())
		{
			std::cout << "Creating world from encrypted config: " << worldName << std::endl;

			// Удаляем старый World если есть
			if (CurrentWorld)
			{
				CurrentWorld = nullptr;
			}

			// Создаем новый World с именем из конфига
			auto WorldUP = std::make_unique<CWorld>(this, worldName);
			CWorld* newWorld = static_cast<CWorld*>(this->AddOwnedObject(std::move(WorldUP)));
			SetWorld(newWorld);

			std::cout << "World '" << worldName << "' created successfully from encrypted config." << std::endl;
		}
		else
		{
			std::cout << "No valid world configuration found in encrypted file." << std::endl;
			return false;
		}

		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR loading encrypted config: " << e.what() << std::endl;
		return false;
	}
}


std::string CGameInstance::LoadNameFromConfig()
{
	try
	{
		std::ifstream configFile(m_ConfigFilePath, std::ios::binary);
		if (!configFile.is_open())
		{
			// Файла нет - возвращаем пустую строку
			return "";
		}

		// Читаем зашифрованные данные
		std::string encryptedData(
			(std::istreambuf_iterator<char>(configFile)),
			std::istreambuf_iterator<char>()
		);
		configFile.close();

		// Дешифруем
		std::string plainText = Encript(encryptedData, MyKey);

		// Парсим только имя GameInstance
		std::stringstream ss(plainText);
		std::string line;
		std::string currentSection;

		while (std::getline(ss, line))
		{
			if (line.empty() || line[0] == '#')
				continue;

			if (line[0] == '[' && line.back() == ']')
			{
				currentSection = line.substr(1, line.length() - 2);
				continue;
			}

			if (currentSection == "GameInstance")
			{
				size_t equalsPos = line.find('=');
				if (equalsPos != std::string::npos)
				{
					std::string key = line.substr(0, equalsPos);
					std::string value = line.substr(equalsPos + 1);

					// Убираем пробелы
					key.erase(0, key.find_first_not_of(" \t"));
					key.erase(key.find_last_not_of(" \t") + 1);
					value.erase(0, value.find_first_not_of(" \t"));
					value.erase(value.find_last_not_of(" \t") + 1);

					if (key == "Name")
					{
						return value; // Нашли имя, возвращаем
					}
				}
			}
		}

		// Имя не найдено в конфиге
		return "";
	}
	catch (...)
	{
		// Любая ошибка - возвращаем пустую строку
		return "";
	}
}

std::string CGameInstance::Encript(const std::string& data, const std::string& key)
{
	std::string result = data;
	for (size_t i = 0; i < data.size(); ++i) {
		result[i] = data[i] ^ key[i % key.size()];
	}
	return result;
}

bool CGameInstance::CheckConfigFile()
{
	// Простая проверка с помощью ifstream
	std::ifstream configFile(m_ConfigFilePath);
	if (configFile.good())
	{
		configFile.close();
		std::cout << "SUCCESS: Config file '" << m_ConfigFilePath << "' found." << std::endl;
		return true;
	}
	
	std::cout << "Config file '" << m_ConfigFilePath << "' not found." << std::endl;

	// Простое сообщение без проверки директории
	std::cout << "Looking for file at: " << m_ConfigFilePath << std::endl;

	return false;
}

CGameInstance& CGameInstance::GetInstance()
{
	if (!Instance)
	{
		std::cerr << "Fatal Error: GameInstance not created! "
			<< "Call CreateInstance() before GetInstance()." << std::endl;
		std::terminate();
	}
	return *Instance;
}

CGameInstance* CGameInstance::GetInstancePtr()
{
	return Instance.get();
}

bool CGameInstance::SaveConfig()
{
	try
	{
		fs::path configPath = fs::current_path() / m_ConfigFilePath;

		std::cout << "Saving encrypted config to: " << configPath.string() << std::endl;

		// Формируем обычный текст конфига
		std::stringstream configStream;
		configStream << "# Configuration file for ChuddoEngine\n";
		configStream << "# This file is automatically generated\n\n";
		configStream << "[GameInstance]\n";
		configStream << "Name = " << this->GetName() << "\n";
		configStream << "Initialized = " << (bIsInitialized ? "true" : "false") << "\n";

		configStream << "\n[World]\n";
		if (this->GetWorld())
		{
			configStream << "Name = " << this->GetWorld()->GetName() << "\n";
			configStream << "Exists = true\n";
		}
		else
		{
			configStream << "Name = None\n";
			configStream << "Exists = false\n";
		}

		std::string plainText = configStream.str();

		// Шифруем текст
		std::string encrypted = Encript(plainText, MyKey);

		// Сохраняем зашифрованный файл
		std::ofstream configFile(configPath, std::ios::binary);
		configFile.write(encrypted.data(), encrypted.size());
		configFile.close();

		std::cout << "Encrypted config successfully saved." << std::endl;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR saving config: " << e.what() << std::endl;
		return false;
	}
}

bool CGameInstance::CreateInstance(const std::string& Name)
{
	if (bInstanceCreated)
	{
		std::cerr << "Warning: GameInstance already created! "
			<< "Ignoring duplicate creation." << std::endl;
		return false;
	}

	try
	{
		Instance = std::unique_ptr<CGameInstance>(new CGameInstance(Name));
		bInstanceCreated = true;


		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error creating GameInstance: " << e.what() << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << "Unknown error creating GameInstance!" << std::endl;
		return false;
	}
}

void CGameInstance::DestroyInstance()
{
	if (!Instance)
	{
		std::cout << "GameInstance already destroyed or never created." << std::endl;
		return;
	}

	if (Instance->bIsInitialized)
	{
		Instance->Shutdown();
	}


	Instance.reset();
	bInstanceCreated = false;
}

bool CGameInstance::Init()
{
	if (bIsInitialized)
	{
		std::cout << "GameInstance '" << GetName() << "' already initialized." << std::endl;
		return true;
	}

	std::cout << "Initializing GameInstance '" << GetName() << "'..." << std::endl;

	try
	{
		//check has saved file if not run DefaultInitialization
		if (!CheckConfigFile()) //check here
		{
			std::cout << "Config file not created init default\n";
			DefaultInitialization();
			bIsInitialized = true;
			std::cout << "GameInstance '" << GetName() << "' initialized successfully." << std::endl;
			return true;
		}
		else //stub
			{
			DefaultInitialization ();
			bIsInitialized = true;
			std::cout << "GameInstance '" << GetName () << "' initialized successfully." << std::endl;
			return true;
			}
		// Здесь должна быть ваша логика инициализации при наличии файла конфигурации:
		// 1. Загрузка конфигурации
		if (!LoadFromConfig())
		{
			std::cout << "failed to load config...\n";
			std::cout << "Load default game instance...\n";
			DefaultInitialization();
		}
		// 2. Инициализация подсистем
		// 3. Создание менеджеров ресурсов
		// 4. И т.д.

		//stub

		bIsInitialized = true;
		std::cout << "GameInstance '" << GetName() << "' initialized successfully." << std::endl;
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to initialize GameInstance: " << e.what() << std::endl;
		bIsInitialized = false;
		return false;
	}
}

void CGameInstance::Shutdown()
{
	if (!bIsInitialized)
	{
		std::cout << "GameInstance '" << GetName() << "' not initialized, nothing to shutdown." << std::endl;
		return;
	}


	std::cout << "Shutting down GameInstance '" << GetName() << "'..." << std::endl;

	try
	{
		// Здесь должна быть ваша логика завершения:
		// 1. Сохранение состояния
		// 2. Очистка ресурсов
		// 3. Уничтожение подсистем
		// 4. И т.д.

		bIsInitialized = false;
		std::cout << "GameInstance '" << GetName() << "' shutdown complete." << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error during GameInstance shutdown: " << e.what() << std::endl;
	}
}