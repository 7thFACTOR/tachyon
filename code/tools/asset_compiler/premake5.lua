project "asset_compiler"
	kind "ConsoleApp"
	defines {"B_IMPORTS", "E_IMPORTS"}
	defines {"_CONSOLE"}
	debugargs { "../assets/engine" }
	add_sources_from("./")
	add_res_from("./")
	add_binpack()
	add_stb_image()
	vpaths { ["*"] = scriptRoot.."/tools/asset_compiler/**.*" }
	link_win32()
	link_base()
	link_engine()
	link_cg()
	link_fbxsdk_static()
	link_squish_static()
	link_freetype_static()