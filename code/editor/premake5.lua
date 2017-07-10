project "editor"
	filter {}
	kind "ConsoleApp"
	defines {"B_IMPORTS", "E_IMPORTS"}
	defines {"_CONSOLE"}
	defines {"EDITOR_CORE_STATIC"}
	add_sources_from("./")
	add_res_from("./")
	link_win32()
	link_base()
	link_engine()