// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include <stdarg.h>
#include <stdio.h>

#ifdef E_STATIC
	#define E_API
	#define E_API_IMPORT
	#define E_API_EXPORT
	#define E_CAPI_IMPORT
	#define E_CAPI_EXPORT
#else 
	#ifdef _WINDOWS
		#ifdef E_EXPORTS
			#define E_API __declspec(dllexport)
		#else
			#define E_API __declspec(dllimport)
		#endif
	#else
		#ifdef E_EXPORTS
			#define E_API __attribute__((dllexport))
		#else
			#define E_API __attribute__(dllimport)
		#endif
	#endif

	#ifdef _WINDOWS
		#define E_API_EXPORT extern "C++" __declspec(dllexport)
		#define E_API_IMPORT extern "C++" __declspec(dllimport)
		#define E_CAPI_EXPORT extern "C" __declspec(dllexport)
		#define E_CAPI_IMPORT extern "C" __declspec(dllimport)
	#else
		#define E_API_EXPORT extern "C++" __attribute__((dllexport))
		#define E_API_IMPORT extern "C++" __attribute__((dllimport))
		#define E_CAPI_EXPORT extern "C" __attribute__((dllexport))
		#define E_CAPI_IMPORT extern "C" __attribute__((dllimport))
	#endif
#endif

//! use this define for exported functions: E_EXPORT void someFunc();
#define E_EXPORT extern "C++" E_API

#define E_REGISTER_CLASS(className) B_REGISTER_RUNTIME_CLASS(engine::getClassRegistry(), className);
#define E_CODE_MARKER(name) engine::getCodeCovergae().hitMarker(""#name);

