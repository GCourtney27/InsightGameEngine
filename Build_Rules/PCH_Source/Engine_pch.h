#pragma once

// === C/C++ Standard Library === //
#include <map>
#include <array>
#include <stack>
#include <queue>
#include <vector>
#include <future>
#include <string>
#include <math.h>
#include <thread>
#include <memory>
#include <sstream>
#include <utility>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

// === Third Party === //
// Rapid Json
#include <rapidjson/json.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/filewritestream.h>

// === Insight Specific === //
#include "Insight/Core/Log.h"
#include "Insight/Core/Interfaces.h"
#include "Insight/Math/ie_Vectors.h"
#include "Insight/Core/ie_Exception.h"
#include "Insight/Utilities/Profiling.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Utilities/String_Helper.h"


#if _MSC_VER >= 1700
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;
using std::static_pointer_cast;
using std::dynamic_pointer_cast;
using std::make_shared;
using std::make_unique;
#elif _MSC_VER == 1600
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::unique_ptr;
using std::tr1::static_pointer_cast;
using std::tr1::dynamic_pointer_cast;
#endif


// -----------
// Platforms |
// -----------

// ---------------------------------
//		Shared Windows Platform		|
// ---------------------------------
// Some files can be shared between UWP and Win32.
#if defined (IE_PLATFORM_WINDOWS)

	// Windows
	#include <wrl/client.h>

	// Direct3D 11
	//#include <d3d11.h>

	// Direct3D 12
	#include <d2d1_3.h>
	#include <dwrite.h>
	#include <d3d11on12.h>
	#include <d3d12.h>
	#include <DirectX12/d3dx12.h> 

	// DirectX
	#if defined(NTDDI_WIN10_RS2)
	#include <dxgi1_6.h>
	#else
	#include <dxgi1_5.h>
	#endif
	#include <wincodec.h>
	#include <DirectXMath.h>
	#include <d3dcompiler.h>
	#if defined (IE_DEBUG)
		#include <dxgidebug.h>
	#endif


#define TrackGraphicsEvents 1

#if TrackGraphicsEvents
#include <WinPixEventRuntime/pix3.h>
#define BeginTrackRenderEvent(pCommandList, Color, Tag) PIXBeginEvent(pCommandList, Color, Tag);
#define EndTrackRenderEvent(pCommandList) PIXEndEvent(pCommandList);
#else
#define BeginTrackRenderEvent(pCommandList, Color, Tag)
#define EndTrackRenderEvent(pCommandList)
#endif

#endif // IE_PLATFORM_WINDOWS


// ---------------
//		Win32	  |
// ---------------
#if defined (IE_PLATFORM_BUILD_WIN32)

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers.
	#endif

	// Windows API	
	#include <shlobj.h>
	#include <Windows.h>
	#include <strsafe.h>
	#include <Shlwapi.h>
	#include <windowsx.h>
	
#endif // IE_PLATFORM_BUILD_WIN32


// ---------------------------------------------
//		Universal Windows Platform (UWP)		|
// ---------------------------------------------
#if defined (IE_PLATFORM_BUILD_UWP)

	// Windows Runtime
	#include "winrt/Windows.System.h"
	//#include "winrt/Windows.Storage.h"
	#include "winrt/Windows.UI.Core.h"
	#include "winrt/Windows.UI.Input.h"
	#include "winrt/Windows.Foundation.h"
	//#include "winrt/Windows.Storage.Pickers.h"
	#include "winrt/Windows.ApplicationModel.h"
	#include "winrt/Windows.Graphics.Display.h"
	#include "winrt/Windows.UI.ViewManagement.h"
	#include "winrt/Windows.ApplicationModel.Core.h"
	#include "winrt/Windows.ApplicationModel.Activation.h"


#endif // IE_PLATFORM_BUILD_UWP
