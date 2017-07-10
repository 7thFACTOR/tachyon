project "sample_module"
	kind "SharedLib"
	defines {"B_IMPORTS", "E_IMPORTS"}
	defines {"_CONSOLE"}
	add_sources_from("./")
	link_win32()
	link_base()
	link_engine()
	
	if _OS == "windows" then
		postbuildcommands { "xcopy /y \"$(TargetPath)\" $(SolutionDir)\\..\\bin\\modules\\" }
	end