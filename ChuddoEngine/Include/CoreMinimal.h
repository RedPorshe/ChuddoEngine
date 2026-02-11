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
#include < chrono >

//engine
#include "Utils/Math/MathTypes.h"
#include "Utils/Logger.h"
#include "Core/Collision.h"
// Forward declarations
class CObject;
class CObjectFactory;

// Global factory access
extern CObjectFactory & OBJECT_FACTORY;

#define EDITOR_MODE 0

#ifdef EDITOR_MODE
#define IN_EDITOR 1
#else
#define IN_EDITOR 0
#endif // EDITOR_MODE