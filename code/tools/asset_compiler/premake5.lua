project "asset_compiler"
	filter { "configurations:Shipping or ShippingEditor", "system:windows" }
		kind "WindowedApp"
		entrypoint ""
	filter { "configurations:Debug or DebugEditor or Development or DevelopmentEditor", "system:windows" }
		kind "ConsoleApp"
		defines {"_CONSOLE"}
		entrypoint "wmainCRTStartup"
	filter {}
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