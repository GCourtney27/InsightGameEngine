-- Engine_Source
-- Engine_Source project is a container for the source code. It does not build anything.

rootDirPath = "../"
engineThirdPartyDir = rootDirPath .. "Engine_Source/Third_Party/"
projectName = "Game_Runtime"

gameRuntimeIncludeDirs = {}
gameRuntimeIncludeDirs["assimp"]					= engineThirdPartyDir .. "assimp-5.0.1/include/"
gameRuntimeIncludeDirs["Microsoft"] 				= engineThirdPartyDir .. "Microsoft/"
gameRuntimeIncludeDirs["Nvidia"]					= engineThirdPartyDir .. "Nvidia/"
gameRuntimeIncludeDirs["spdlog"]					= engineThirdPartyDir .. "spdlog/include/"
gameRuntimeIncludeDirs["rapidjson"] 				= engineThirdPartyDir .. "rapidjson/include/"
gameRuntimeIncludeDirs["Mono"]						= monoInstallDir .. "include/"
gameRuntimeIncludeDirs["Engine_Source_Root"]		= rootDirPath .. "Engine_Source/"
gameRuntimeIncludeDirs["Engine_Source_Src"]			= rootDirPath .. "Engine_Source/Source/"
gameRuntimeIncludeDirs["Engine_Source_Third_Party"] = rootDirPath .. "Engine_Source/Third_Party/"
gameRuntimeIncludeDirs["Build_Rules"]				= rootDirPath .. "Build_Rules/"
gameRuntimeIncludeDirs["Game_Runtime"]				= rootDirPath .. "Game_Runtime/Source/"

project (projectName)
	location (rootDirPath .. projectName)
	filter "configurations:*-Package"
		kind ("SharedLib")
	language ("C++")
	cppdialect ("C++17")

	targetdir (rootDirPath .. binaryDirectory .. "%{prj.name}/Staging")
    objdir (rootDirPath .. intDirectory .. "%{prj.name}")

	files
	{
		"Game-Runtime-Make.lua",

		-- Personal Source Files for this Application
		"Source/**.h",
		"Source/**.cpp",
		"./**.h",
		"./**.cpp",

	}

	defines
	{
		"IE_PLATFORM_WINDOWS"
	}

	includedirs
	{
		"%{gameRuntimeIncludeDirs.assimp}",
		"%{gameRuntimeIncludeDirs.Microsoft}",
		"%{gameRuntimeIncludeDirs.Nvidia}DirectX12/",
		"%{gameRuntimeIncludeDirs.Microsoft}WinPixEventRuntime/Include/",
		"%{gameRuntimeIncludeDirs.spdlog}",
		"%{gameRuntimeIncludeDirs.rapidjson}",
		"%{gameRuntimeIncludeDirs.Mono}mono-2.0/",
		"%{gameRuntimeIncludeDirs.Engine_Source_Src}/",
		"%{gameRuntimeIncludeDirs.Engine_Source_Third_Party}/",
		
		-- PCH Source
		"%{gameRuntimeIncludeDirs.Build_Rules}/PCH_Source/",

		-- Engine source
		"%{gameRuntimeIncludeDirs.Engine_Source_Src}/",
		
		-- Personal Source Files for this game
		"Source/",
		"./",
	}

	filter "configurations:not *Package"
	defines { "BUILD_GAME_DLL", "COMPILE_DLL" }
