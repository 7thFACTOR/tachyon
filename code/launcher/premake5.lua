project "launcher"
	kind "WindowedApp"
	defines {"B_IMPORTS", "E_IMPORTS"}
	--defines {"_CONSOLE"}
	entrypoint ""
	add_sources_from("./")
	add_res_from("./")
	link_win32()
	link_base()
	link_engine()

project "launcher_s"
	kind "WindowedApp"
	defines {"B_STATIC", "E_STATIC"}
	--defines {"_CONSOLE"}
	entrypoint ""
	add_sources_from("./")
	add_res_from("./")
	link_win32()
	link_base_static()
	link_engine_static()