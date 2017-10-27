project "launcher"
	defines {"B_IMPORTS", "E_IMPORTS"}
	filter { "configurations:Shipping or ShippingEditor", "system:windows" }
		kind "WindowedApp"
		entrypoint ""
	filter { "configurations:Debug or DebugEditor or Development or DevelopmentEditor", "system:windows" }
		kind "ConsoleApp"
		defines {"_CONSOLE"}
		entrypoint "wmainCRTStartup"
	filter {}
	add_sources_from("./")
	add_res_from("./")
	link_win32()
	link_base()
	link_engine()

project "launcher_s"
	defines {"B_STATIC", "E_STATIC"}
	filter { "configurations:Shipping or ShippingEditor", "system:windows" }
		kind "WindowedApp"
		entrypoint ""
	filter { "configurations:Debug or DebugEditor or Development or DevelopmentEditor", "system:windows" }
		kind "ConsoleApp"
		defines {"_CONSOLE"}
		entrypoint "wmainCRTStartup"
	filter {}
	add_sources_from("./")
	add_res_from("./")
	link_win32()
	link_base_static()
	link_engine_static()