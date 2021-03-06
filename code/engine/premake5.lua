project "engine"
	kind "SharedLib"
	defines {"E_EXPORTS", "B_IMPORTS"}
	add_sources_from("./")
	add_minizip()
	vpaths { ["*"] = scriptRoot.."/engine/**.*" }
	excludes {"physics/physx/**"}
	includedirs {scriptRoot.."/engine"}
	link_glew_static()
	link_zlib_static()
	link_sfml_static()
	link_win32()
	link_base()
	link_physx()
	link_opengl()

project "engine_s"
	kind "StaticLib"
	defines {"E_STATIC"}
	defines {"B_STATIC"}
	add_sources_from("./")
	add_minizip()
	vpaths { ["*"] = scriptRoot.."/engine/**.*" }
	excludes {"physics/physx/**"}
	includedirs {scriptRoot.."/engine"}
	link_glew_static()
	link_zlib_static()
	link_base_static()
	link_sfml_static()
	link_win32()
	link_physx()
	link_opengl()
