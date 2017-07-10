project "launcher"
	kind "ConsoleApp"
	defines {"B_IMPORTS", "E_IMPORTS"}
	defines {"_CONSOLE"}
	add_sources_from("./")
	add_res_from("./")
	link_win32()
	link_base()
	link_engine()

project "launcher_s"
	kind "ConsoleApp"
	defines {"B_STATIC", "E_STATIC"}
	defines {"_CONSOLE"}
	add_sources_from("./")
	add_res_from("./")
	link_win32()
	link_base_static()
	link_engine_static()