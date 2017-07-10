project "base"
	kind "SharedLib"
	defines {"B_EXPORTS"}
	includedirs {scriptRoot}
	add_sources_from("./")
	add_binpack()
	vpaths { ["*"] = scriptRoot.."/base/**.*" }
	link_win32()
	link_freetype_static()

project "base_s"
	kind "StaticLib"
	defines {"B_STATIC"}
	includedirs {scriptRoot}
	add_sources_from("./")
	add_binpack()
	vpaths { ["*"] = scriptRoot.."/base/**.*" }
	link_win32()
	link_freetype_static()