#include <Roose/Core/PlatformDetection.h>

#ifdef RS_PLATFORM_WINDOWS
    #ifdef _MSC_VER
        #define EXPORT_SYMBOL __declspec(dllexport)
    #else
        #define EXPORT_SYMBOL __attribute__((visibility("default")))
    #endif

    // Enforces the use of the high-performance GPU on systems with hybrid graphics (NVIDIA Optimus or AMD PowerXpress).
    extern "C" {
        EXPORT_SYMBOL unsigned int NvOptimusEnablement = 0x00000001;
        EXPORT_SYMBOL int AmdPowerXpressRequestHighPerformance = 1;
    }
#endif
