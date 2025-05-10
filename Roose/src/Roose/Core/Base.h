#pragma once

#include "Roose/Core/PlatformDetection.h"

#include <memory>

#ifdef RS_DEBUG
	#if defined(RS_PLATFORM_WINDOWS)
		#define RS_DEBUGBREAK() __debugbreak()
	#elif defined(RS_PLATFORM_LINUX)
		#include <signal.h>
		#define RS_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define RS_ENABLE_ASSERTS
#else
	#define RS_DEBUGBREAK()
#endif

#define RS_EXPAND_MACRO(x) x
#define RS_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << (x))

#define RS_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Roose {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

#include "Roose/Core/Assert.h"
