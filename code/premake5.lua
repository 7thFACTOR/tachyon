workspace "tachyon"

-- Location of the solutions
if _ACTION == "vs2015" then
	location "../build"
	-- We're on Windows, this will be used in code for ifdef
	defines {"_WINDOWS"}
	system ("windows")
	-- Setup post build to copy the binaries to /bin
	--postbuildcommands { "xcopy /y \"$(TargetPath)\" $(SolutionDir)\\..\\bin\\" }
end

if _ACTION == "gmake" then
	location "../build_gmake"
	-- We're on Linux, this will be used in code for ifdef
	defines {"_LINUX"}
	system ("linux")
end

if _ACTION == "xcode4" then
	location "../build_xcode"
	-- We're on OSX, this will be used in code for ifdef
	defines {"_APPLE"}
	system ("apple")
end

postbuildcommands { "mkdir -p ../bin" }
postbuildcommands { "{COPY} \"%{cfg.buildtarget.abspath}\" ../bin" }

filter { "configurations:Debug or DebugEditor", "system:windows" }
	postbuildcommands { "{COPY} \"%{cfg.buildtarget.directory}\%{prj.name}.pdb\" ../bin" }
filter {}

language "C++"

-- We only support 64bit architectures
architecture "x64"

-- Configurations for the build:
-- Debug: the engine, modules, with debug information, but with no editor support
-- Development: the engine, modules, with minimal debug information, faster, but with no editor support
-- Shipping: the engine, modules, release mode, optimized for speed, but with no editor support
-- DebugEditor: the editor, engine, modules, with debug information, with editor support
-- DevelopmentEditor: the editor, engine, modules, with debug information, with editor support
-- Shipping: the editor, engine, modules, release mode, optimized for speed, with editor support
configurations { "Debug", "Development", "Shipping", "DebugEditor", "DevelopmentEditor", "ShippingEditor" }

-- Keep the script root to be used in paths
scriptRoot = _WORKING_DIR

-- With no filter, we set some global settings
filter {}
debugdir "../bin"

-- We supoort utf8
characterset ("MBCS")

flags { "NoMinimalRebuild", "MultiProcessorCompile", "NoPCH", "C++11" }
warnings "Off"

filter { "system:windows" }
	-- Disable some warnings
	disablewarnings { 4251, 4006, 4221, 4204 }
filter {}

filter { "system:linux or macos or ios" }
	buildoptions {"-pthread"}

filter {}

---
--- Prepare the configurations
---

filter { "configurations:Debug" }
	symbols "On"
	optimize "Debug"
	defines {"_DEBUG"}
	
filter { "configurations:DebugEditor" }
	symbols "On"
	optimize "Debug"
	defines {"_DEBUG", "_EDITOR"}

filter { "configurations:Development" }
	symbols "On"
	optimize "Speed"
	defines {"_DEVELOPMENT"}
	defines {"NDEBUG"}

filter { "configurations:DevelopmentEditor" }
	symbols "On"
	optimize "Speed"
	defines {"_DEVELOPMENT"}
	defines {"NDEBUG", "_EDITOR"}
	
filter { "configurations:Shipping" }
	optimize "Full"
	defines {"_SHIPPING"}
	defines {"NDEBUG"}
	
filter { "configurations:ShippingEditor" }
	optimize "Full"
	defines {"_SHIPPING", "_EDITOR"}
	defines {"NDEBUG"}

---
--- Global defines
---
filter {}

-- used for new/delete override, with logging of new/delete operations
filter { "configurations:Debug or DebugEditor or Development or DevelopmentEditor" }
	defines {"B_USE_MEMORY_OPERATORS_LOG"}
	defines {"B_USE_CODE_COVERAGE"}
	defines {"B_USE_PERFORMANCE_PROFILER"}
	defines {"B_USE_JOB_PROFILING"}
filter {}

---
--- Utility functions
---
	
function add_sources_from(path)
	files { path.."**.h" }
	files { path.."**.hpp" }
	files { path.."**.cpp" }
	files { path.."**.cxx" }
	files { path.."**.c" }
	files { path.."**.inl" }
end

function add_res_from(path)
	files { path.."**/resource.h" }
	files { path.."**.res" }
	files { path.."**.rc" }
end

---
--- Linking with our libraries and 3rdparty
---

function link_win32()
	filter { "system:windows" }
		links { "shlwapi", "winmm",  "Ws2_32", "Wininet", "dbghelp" }
	filter {}
end

function link_opengl()
	filter {}
	defines {"USE_OPENGL"}

	filter { "system:windows" }
		links { "opengl32" }

	filter { "system:linux" }
		defines {"USE_OPENGL"}
		links { "GL" }
		
	filter {}
end

function link_freetype_static()
	filter {}
	includedirs {scriptRoot.."/3rdparty/freetype/include"}

	filter { "system:windows" }
		libdirs { scriptRoot.."/3rdparty/freetype/objs/vc2015/x64" }
		links { "freetype28MT" }
		
	filter { "system:linux" }
		libdirs { scriptRoot.."/3rdparty/freetype/objs/" }
		links { "freetype28MT" }
	filter {}
end

function link_sfml_static()
	filter {}
	defines {"SFML_STATIC"}
	defines {"USE_SFML_WINDOW"}
	defines {"USE_SFML_INPUT"}
	defines {"USE_SFML_NETWORK"}
	includedirs {"../3rdparty/SFML/include"}

	filter { "configurations:Debug or DebugEditor", "system:windows" }
		libdirs { "../3rdparty/SFML/lib" }
		links { "sfml-system-s-d", "sfml-network-s-d", "sfml-audio-s-d", "sfml-window-s-d", "openal32", "ogg", "vorbis", "vorbisenc", "vorbisfile", "flac" }
		
	filter { "configurations:Development or DevelopmentEditor or Shipping or ShippingEditor", "system:windows" }
		libdirs { "../3rdparty/SFML/lib" }
		links { "sfml-system-s", "sfml-network-s", "sfml-audio-s", "sfml-window-s", "openal32", "ogg", "vorbis", "vorbisenc", "vorbisfile", "flac" }

	filter { "system:linux" }
		libdirs { "../3rdparty/SFML/lib" }
		links { "libsfml-system-s", "libsfml-network-s", "libsfml-audio-s", "libsfml-window-s" }
		
	filter {}
end

function link_zlib_static()
	print("Linking ZLib...")
	filter {}
	defines {"USE_ZLIB", "ZLIB_STATIC"}
	includedirs {scriptRoot.."/3rdparty/zlib/"}
	
	filter { "system:windows" }
		libdirs { scriptRoot.."/3rdparty/zlib/contrib/vstudio/vc14/x64/ZlibStatReleaseWithoutAsm" }
		links { "zlibstat" }
	
	filter { "system:linux" }
		libdirs { scriptRoot.."/3rdparty/zlib/" }
		links { "zlibstat" }

	filter {}
end

function link_squish_static()
	print("Linking Squish...")
	filter {}
	defines {"USE_SQUISH"}
	includedirs {scriptRoot.."/3rdparty/squish"}

	filter { "configurations:Debug or DebugEditor", "system:windows" }
		libdirs { scriptRoot.."/3rdparty/squish/lib64" }
		links { "squishd" }
	
	filter { "configurations:Development or DevelopmentEditor or Shipping or ShippingEditor", "system:windows" }
		libdirs { scriptRoot.."/3rdparty/squish/lib64" }
		links { "squish" }

	filter { "system:linux" }
		libdirs { scriptRoot.."/3rdparty/squish/lib" }
		links { "squish" }

	filter {}
end

function link_fbxsdk_static()
	print("Linking FbxSDK...")
	filter {}
	includedirs {scriptRoot.."/3rdparty/fbx/include"}

	filter { "configurations:Debug or DebugEditor", "system:windows" }
		libdirs { scriptRoot.."/3rdparty/fbx/lib/vs2015/x64/debug" }
		links { "libfbxsdk-md" }

	filter { "configurations:Development or DevelopmentEditor or Shipping or ShippingEditor", "system:windows" }
		libdirs { scriptRoot.."/3rdparty/fbx/lib/vs2015/x64/release" }
		links { "libfbxsdk-md" }
	
	filter { "system:linux" }
		libdirs { scriptRoot.."/3rdparty/fbx/lib" }
		links { "libfbxsdk" }

	filter {}
end

function link_cg()
	print("Linking Cg...")

	filter {}
	includedirs {scriptRoot.."/3rdparty/Cg/include"}

	filter { "system:windows" }
		libdirs { scriptRoot.."/3rdparty/Cg/lib" }
		links { "cg", "cgGL" }
	
	filter { "system:linux" }
		libdirs { scriptRoot.."/3rdparty/Cg/lib" }
		links { "libCg", "libCgGL" }

	filter {}
end

function link_physx()
	filter {}
	defines {"USE_PHYSX"}
	includedirs {"../3rdparty/physx/PxShared/include/foundation"}
	includedirs {"../3rdparty/physx/PhysX_3.4/Include"}
	includedirs {"../3rdparty/physx/PhysX_3.4/Include/cooking"}
	includedirs {"../3rdparty/physx/PhysX_3.4/Include/characterkinematic"}
	
	filter { "system:windows" }
		libdirs { "../3rdparty/physx/PhysX_3.4/Lib/vc14win64" }
		libdirs { "../3rdparty/physx/PxShared/lib/vc14win64" }
		links { "PhysX3Gpu_x64", "PxCudaContextManager_x64" }
	filter {}
	
	filter { "system:linux" }
		libdirs { "../3rdparty/physx/PhysX_3.4/Bin/linux64" }
		libdirs { "../3rdparty/physx/PxShared/lib/linux64" }
		links { "PhysX3Gpu_x64", "libPxCudaContextManager" }
	filter {}
end

function link_glew_static()
	filter {}
	defines {"GLEW_STATIC"}
	includedirs {scriptRoot.."/3rdparty/glew/include"}

	filter { "system:windows" }
		libdirs { "../3rdparty/glew/lib/Release/x64" }
		links { "glew32s" }
	filter {}
	
	filter { "system:linux" }
		libdirs { "../3rdparty/glew/linux/lib" }
		links { "libglew" }
	filter {}
end

---
--- Own libs
---

function link_base()
	includedirs {scriptRoot}
	links { "base" }
end

function link_base_static()
	includedirs {scriptRoot}
	links { "base_s" }
end

function link_engine()
	includedirs {scriptRoot.."/engine"}
	links { "engine" }
end

function link_engine_static()
	includedirs {scriptRoot.."/engine"}
	links { "engine_s" }
end

---
--- Add code files directly to projects
---

function add_binpack()
	add_sources_from(scriptRoot.."/3rdparty/binpack/")
	-- map some folder filters for VS
	vpaths { ["3rdparty/binpack"] = scriptRoot.."/3rdparty/binpack/**.*" }
end

function add_minizip()
	filter {}
	includedirs {scriptRoot.."/3rdparty/zlib/"}
	files { scriptRoot.."/3rdparty/zlib/contrib/minizip/zip.c" }
	files { scriptRoot.."/3rdparty/zlib/contrib/minizip/ioapi.c" }
	-- map some folder filters for VS
	vpaths { ["3rdparty/minizip"] = scriptRoot.."/3rdparty/zlib/contrib/minizip/**.*" }
	filter { "system:windows" }
		files {scriptRoot.."/3rdparty/zlib/contrib/minizip/iowin32.c" }
	filter {}
end

function add_stb_image()
	add_sources_from(scriptRoot.."/3rdparty/stb_image/")
	-- map some folder filters for VS
	vpaths { ["3rdparty/stb_image"] = scriptRoot.."/3rdparty/stb_image/**.*" }
end

---
--- Include the subprojects
---

-- TODO: we need to generate solutions for CMake for 3rdparty that uses it
--include "3rdparty"	
include "base"
include "engine"
include "editor"
include "launcher"
include "modules"
include "tests"
include "tools"
