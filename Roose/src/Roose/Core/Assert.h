#pragma once

#include "Roose/Core/Log.h"

#include <filesystem>

#ifdef RS_ENABLE_ASSERTS
	#define RS_INTERNAL_ASSERT_IMPL(check, msg, ...) { if(!(check)) { RS_ERROR(msg, __VA_ARGS__); RS_DEBUGBREAK(); } }
	#define RS_INTERNAL_ASSERT_WITH_MSG(check, ...) RS_INTERNAL_ASSERT_IMPL(check, "Assertion failed: %s", __VA_ARGS__)
	#define RS_INTERNAL_ASSERT_NO_MSG(check) RS_INTERNAL_ASSERT_IMPL(check, "Assertion '%s' failed at %s:%d", RS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string().c_str(), __LINE__)

	#define RS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define RS_INTERNAL_ASSERT_GET_MACRO(...) RS_EXPAND_MACRO( RS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RS_INTERNAL_ASSERT_WITH_MSG, RS_INTERNAL_ASSERT_NO_MSG) )

	#define RS_ASSERT(...) RS_EXPAND_MACRO( RS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
	#define RS_ASSERT(...)
#endif
