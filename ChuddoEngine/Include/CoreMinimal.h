#pragma once
//system

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <iostream>

//engine
#include "Utils/Math/MathTypes.h"

#define EDITOR_MODE 0


#ifdef EDITOR_MODE
#define IN_EDITOR 1
#else
#define IN_EDITOR 0
#endif // EDITOR_MODE
