#pragma once

namespace editor
{
#ifdef EDITOR_CORE_STATIC
	#define EDITOR_API
	#define EDITOR_API_IMPORT
	#define EDITOR_API_EXPORT
#else 

#ifdef _WINDOWS
	#ifdef EDITOR_CORE_EXPORTS
		#define EDITOR_API __declspec(dllexport)
	#else
		#define EDITOR_API __declspec(dllimport)
	#endif
#else
	#ifdef EDITOR_CORE_EXPORTS
		#define EDITOR_API __attribute__((dllexport))
	#else
		#define EDITOR_API __attribute__(dllimport)
	#endif
#endif

#ifdef _WINDOWS
	#define EDITOR_API_EXPORT __declspec(dllexport)
	#define EDITOR_API_IMPORT __declspec(dllimport)
#else
	#define EDITOR_API_EXPORT __attribute__((dllexport))
	#define EDITOR_API_IMPORT __attribute__((dllimport))
#endif

#endif

//! use this define for exported functions
#define EDITOR_EXPORT extern "C++" EDITOR_API
#define EDITOR_MODULE_FILE_MASK "*."##B_LIBRARY_EXTENSION

}