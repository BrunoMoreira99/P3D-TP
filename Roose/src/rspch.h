#pragma once

#include "Roose/Core/PlatformDetection.h"

#ifdef RS_PLATFORM_WINDOWS
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#endif

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "Roose/Core/Base.h"

#ifdef RS_PLATFORM_WINDOWS
    #include <Windows.h>
#endif
