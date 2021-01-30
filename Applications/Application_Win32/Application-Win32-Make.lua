-- Application
-- The client exe that gets executed.

appName = "Application"
projectName = appName .. "_Win32"

rootDirPath = "../../"
engineThirdPartyDir = rootDirPath .. "Engine_Source/Third_Party/"
monoInstallDir = "C:/Program Files/Mono/"

win32AppIncludeDirs = {}
win32AppIncludeDirs["assimp"]					= engineThirdPartyDir .. "assimp-5.0.1/include/"
win32AppIncludeDirs["Microsoft"] 				= engineThirdPartyDir .. "Microsoft/"
win32AppIncludeDirs["Nvidia"]					= engineThirdPartyDir .. "Nvidia/"
win32AppIncludeDirs["spdlog"]					= engineThirdPartyDir .. "spdlog/include/"
win32AppIncludeDirs["rapidjson"] 				= engineThirdPartyDir .. "rapidjson/include/"
win32AppIncludeDirs["Mono"]						= monoInstallDir .. "include/"
win32AppIncludeDirs["Engine_Source_Root"]		= rootDirPath .. "Engine_Source/"
win32AppIncludeDirs["Engine_Source_Src"]		= rootDirPath .. "Engine_Source/Source/"
win32AppIncludeDirs["Engine_Source_Third_Party"]= rootDirPath .. "Engine_Source/Third_Party/"
win32AppIncludeDirs["Build_Rules"]				= rootDirPath .. "Build_Rules/"
win32AppIncludeDirs["Game_Runtime"]				= rootDirPath .. "Game_Runtime/Source/"

project (projectName)
	location (rootDirPath .. "Applications/" .. projectName)
	kind ("WindowedApp")
	language ("C++")
	cppdialect ("C++17")
	staticruntime ("off")
	targetname (projectName)
	
	targetdir (rootDirPath .. binaryDirectory .. "%{prj.name}")
    objdir (rootDirPath .. intDirectory .. "%{prj.name}")
	debugdir ("%{cfg.targetdir}")

	files
	{
		"Application-Win32-Make.lua",

		-- Personal Source Files for this Application
		"Source/**.h",
		"Source/**.cpp",
		"./**.h",

		"./**.rc",
	}

	includedirs
	{		
		"%{win32AppIncludeDirs.assimp}",
		"%{win32AppIncludeDirs.Microsoft}",
		"%{win32AppIncludeDirs.Nvidia}DirectX12/",
		"%{win32AppIncludeDirs.Microsoft}WinPixEventRuntime/Include/",
		"%{win32AppIncludeDirs.spdlog}",
		"%{win32AppIncludeDirs.rapidjson}",
		"%{win32AppIncludeDirs.Mono}mono-2.0/",
		"%{win32AppIncludeDirs.Engine_Source_Src}/",
		"%{win32AppIncludeDirs.Engine_Source_Third_Party}/",
		
		-- Personal Source Files for this Application
		"Source/",

		"./",

		-- Shared Header Includes for this Project
		"%{win32AppIncludeDirs.Build_Rules}/PCH_Source/",

		-- Runtime for the game
		"%{win32AppIncludeDirs.Game_Runtime}/",
		
	}

	links
	{
        -- Third Party
		"MonoPosixHelper.lib",
		"mono-2.0-sgen.lib",
        "libmono-static-sgen.lib",
		"assimp-vc142-mtd.lib",
        
        -- DirectX/Windows API
		"d3d12.lib",
		"dxgi.lib",
		"d3d11.lib",
		"WinPixEventRuntime.lib",
		"Shlwapi.lib",
		"DirectXTK.lib",
        "DirectXTK12.lib",
		"d2d1.lib",
		"dwrite.lib",
		
		-- Set the Runtime Library to Win32
		"Engine_Build_Win32",

		"Game_Runtime",
	}

	systemversion ("latest")
	defines
	{
		"IE_PLATFORM_BUILD_WIN32",
	}
	flags
	{
		"MultiProcessorCompile"
	}
	postbuildcommands
	{
		-- Assimp
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Debug/assimp-vc142-mtd.dll %{cfg.targetdir}"),
		-- DX11 Debug Layers
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll %{cfg.targetdir}"),
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3DX11d_43.dll %{cfg.targetdir}"),
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11Ref.dll %{cfg.targetdir}"),
		-- Mono
		("{COPY} \"".. monoInstallDir .."/bin/mono-2.0-sgen.dll\" %{cfg.targetdir}"),
		-- PIX
		("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64/WinPixEventRuntime.dll %{cfg.targetdir}"),
		-- Copy over assets
		("{COPY} %{wks.location}Content %{cfg.targetdir}/../Content"),
		-- Copy over default engine assets
		("{COPY} ../../Engine_Source/Assets %{cfg.targetdir}/../Content/Engine"),
		-- Copy the game runtime
		--("{COPY} %{cfg.targetdir}/../Game_Runtime/Game_Runtime.dll %{cfg.targetdir}")
	}



-- Build Configurations

	filter "configurations:Debug"
		defines "IE_DEBUG"
		symbols "on"
		runtime "Debug"
		libdirs
		{
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Debug/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Debug/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Debug/",
			monoInstallDir .. "/lib/",
			"%{cfg.targetdir}/../Game_Runtime/"
		}
	
	filter "configurations:Release"
		defines "IE_RELEASE"
		symbols "on"
		optimize "on"
		libdirs
		{
			"%{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release/",
			"%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release/",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release/",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- Assimp
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release/assimp-vc140-mt.dll %{cfg.targetdir}"),
			-- DX11 Debug Layers	
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11SDKLayers.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3DX11d_43.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/Bin/D3D11Ref.dll %{cfg.targetdir}"),
			-- Mono					
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Mono/bin/mono-2.0-sgen.dll %{cfg.targetdir}"),
			-- DirectX				
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll %{cfg.targetdir}"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			-- PIX					
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/WinPixEventRuntime.1.0.161208001/bin/WinPixEventRuntime_UAP.dll %{cfg.targetdir}"),
			-- Copy over assets
			("{COPY} $(USERPROFILE)/Documents/Insight-Projects/Development-Project/Content/** ../Binaries/" .. outputdir .. "/Content"),
			("{COPY} %{wks.location}/Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}
		
	filter "configurations:GameDist"
		defines "IE_GAME_DIST"
		optimize "on"
		symbols "on"
		libdirs
		{
			"%{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release",            
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/WinPixEventRuntime/bin/x64",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/TK/Bin/Desktop_2019_Win10/x64/Release",
			"%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/DXTex/DirectXTex/Bin/Desktop_2019_Win10/x64/Release",
            "%{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX11/TK/Bin/Desktop_2019_Win10/x64/Release",
			monoInstallDir .. "/lib",
		}
		postbuildcommands
		{
			-- assimp
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/assimp-5.0.1/build/code/Release/assimp-vc140-mt.dll %{cfg.targetdir}"),
			-- mono
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Mono/bin/mono-2.0-sgen.dll %{cfg.targetdir}"),
			-- DirectX
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxcompiler.dll %{cfg.targetdir}"),
			("{COPY} %{win32AppIncludeDirs.Engine_Source_Third_Party}/Microsoft/DirectX12/Bin/dxil.dll %{cfg.targetdir}"),
			("{COPY} %{wks.location}/Engine/Source/Shaders/HLSL/Ray_Tracing/** ../Binaries/" .. outputdir.."/Engine"),
			("{COPY} %{wks.location}/Engine_Source/Assets/Textures/Default_Object/** ../Binaries/"..outputdir.."/Content/Default_Assets/")
		}

	