#pragma once
//system

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <locale>

//engine
#include "Utils/Math/MathTypes.h"

// Вместо включения ObjectFactory.h - только forward declaration
class CObjectFactory;
#define OBJECT_FACTORY CObjectFactory::GetInstance()

#define EDITOR_MODE 0

#ifdef EDITOR_MODE
#define IN_EDITOR 1
#else
#define IN_EDITOR 0
#endif // EDITOR_MODE