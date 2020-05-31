#pragma once

#if defined IE_PLATFORM_WINDOWS
	#if defined IE_DYNAMIC_LINK
		#if defined IE_BUILD_DLL
			#define INSIGHT_API __declspec(dllexport)
		#else
			#define INSIGHT_API __declspec(dllimport)
		#endif
	#else
		#define INSIGHT_API 
	#endif
#endif // IE_PLATFORM_WINDOWS

#if defined IE_ENGINE_DIST || defined IE_GAME_DIST
	#define IE_PRODUCTION
#endif

#if defined IE_DEBUG
	#define IE_ENABLE_ASSERTS
#endif // IE_DEBUG

#if defined IE_ENABLE_ASSERTS
	#define IE_ASSERT(x, ...) {if( !(x) ) { IE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define IE_CORE_ASSERT(x, ...) { if(!(x)) { IE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define IE_ASSERT(x, ...)
	#define IE_CORE_ASSERT(x, ...)
#endif // IE_ENABLE_ASSERTS



// Utilities
#define BIT_SHIFT(x) ( 1 << x )
#define IE_BIND_EVENT_FN(fn) std::bind( &fn, this, std::placeholders::_1 )
#define COM_SAFE_RELEASE(ptr) if(ptr) ptr->Release();
#define CAST_TO_FLOAT(...) static_cast<float>(__VA_ARGS__)
#define CAST_TO_UINT(...) static_cast<UINT>(__VA_ARGS__)
