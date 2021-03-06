#pragma once

// We are compiling for Win32 or UWP we can assume our platform is Windows
#if defined (IE_PLATFORM_BUILD_WIN32) || defined (IE_PLATFORM_BUILD_UWP)
	#define IE_PLATFORM_WINDOWS
#endif

// Only msvc supports __declspec
#if defined (IE_PLATFORM_WINDOWS)
	#if defined IE_DYNAMIC_LINK
		#if defined IE_BUILD_DLL
			#define INSIGHT_API __declspec(dllexport)
		#else
			#define INSIGHT_API __declspec(dllimport)
		#endif
	#else
		#define INSIGHT_API
	#endif
#endif // IE_PLATFORM_BUILD_WIN32

#if defined IE_DEBUG
	#define IE_ENABLE_ASSERTS
	#define IE_SCOPE_PROFILING_ENABLED
#endif // IE_DEBUG

#if defined IE_ENABLE_ASSERTS
	#define IE_ASSERT(x, ...) { if( !(x) ) { IE_DEBUG_LOG(LogSeverity::Error, "Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define IE_ASSERT(x, ...)
#endif // IE_ENABLE_ASSERTS

// Configuration defines
#if defined IE_GAME_DIST
#define IE_STRIP_FOR_GAME_DIST(x)
#define IE_ADD_FOR_GAME_DIST(x) x
#else
#define IE_STRIP_FOR_GAME_DIST(x) x
#define IE_ADD_FOR_GAME_DIST(x)
#endif

// Utilities
#define BIT_SHIFT(x) ( 1 << x )
#define IE_BIND_LOCAL_EVENT_FN(Fn) std::bind( &Fn, this, std::placeholders::_1 )
#define IE_BIND_EVENT_FN(Fn, Class) std::bind( &Fn, Class, std::placeholders::_1 )
#define IE_BIND_LOCAL_VOID_FN(Fn) std::bind( &Fn, this )
#define COM_SAFE_RELEASE(ComObject) if(ComObject) { ComObject->Release(); ComObject = nullptr; }
#define RAW_LITERAL(Value) #Value
#define MACRO_TO_STRING(Macro) RAW_LITERAL(Macro);
#define FORCE_INLINE __forceinline

// Includes
#include "Insight/Math/Math_Helpers.h"

// Rendering
#define MAX_POINT_LIGHTS_SUPPORTED 16u
#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 1u
#define MAX_SPOT_LIGHTS_SUPPORTED 16u
