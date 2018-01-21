// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once

#ifdef _LINUX
#include <signal.h>
#endif

//////////////////////////////////////////////////////////////////////////
// Configuration
//////////////////////////////////////////////////////////////////////////

#ifdef _WINDOWS
	#ifdef _DEBUG
		#define B_DEBUG_BREAK { _CrtDbgBreak(); }
	#else
		#define B_DEBUG_BREAK
	#endif
	#define B_LIBRARY_EXTENSION "dll"
#endif

#if defined(_LINUX) || defined(_OSX)
	#ifdef _DEBUG
		#define B_DEBUG_BREAK { raise(5); }
	#else
		#define B_DEBUG_BREAK
	#endif
	#define B_LIBRARY_EXTENSION "so"
#endif

#ifdef B_STATIC
	#define B_API
	#define B_EXPORT
	#define B_CAPI_IMPORT
	#define B_CAPI_EXPORT
#else 

	#ifdef _WINDOWS
		#ifdef B_EXPORTS
			//! use this define for exported classes: class B_API SomeClass
			#define B_API __declspec(dllexport)
		#else
			#define B_API __declspec(dllimport)
		#endif
	#else
		#ifdef B_EXPORTS
			#define B_API __attribute__((dllexport))
		#else
			#define B_API __attribute__((dllimport))
		#endif
	#endif

	#ifdef _WINDOWS
		#define B_API_EXPORT extern "C++" __declspec(dllexport)
		#define B_API_IMPORT extern "C++" __declspec(dllimport)
		#define B_CAPI_EXPORT extern "C" __declspec(dllexport)
		#define B_CAPI_IMPORT extern "C" __declspec(dllimport)
	#else
		#define B_API_EXPORT extern "C++" __attribute__((dllexport))
		#define B_API_IMPORT extern "C++" __attribute__((dllimport))
		#define B_CAPI_EXPORT extern "C" __attribute__((dllexport))
		#define B_CAPI_IMPORT extern "C" __attribute__((dllimport))
	#endif

	//! use this define for exported functions: B_EXPORT void someFunc();
	#define B_EXPORT extern "C++" B_API
#endif

//! safe delete and object by checking if it is different from nullptr
//! and setting it to nullptr after deletion
#define B_SAFE_DELETE(what) if (nullptr != what){ delete what; what = nullptr; }
//! safe delete and object by checking if it is different from nullptr
//! and setting it to nullptr after deletion
#define B_SAFE_DELETE_ARRAY(what) if (nullptr != what){ delete [] what; what = nullptr; }
//! return a left shifted int
#define B_BIT(n) (1 << (n))
#define B_IS_BIT_SET(whatBit, fromWhere) ((whatBit) == ((fromWhere) & (whatBit)))
#define B_SET_BIT(whatBit, dest, trueOrFalse) ((trueOrFalse) ? (dest) |= (whatBit) : (dest) &= ~(whatBit))
//! checks if what is between start and end
#define B_INBOUNDS(what, start, end) ((what) >= (start) && (what) < (end))
#define B_STRINGIFY(what) #what
#define B_TOSTRING(what) B_STRINGIFY(what)
#define B_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define B_MIN(a,b) (((a) < (b)) ? (a) : (b))

#define B_ENUM_AS_FLAGS(T)\
	B_ENUM_AS_FLAGS_EX(T, u32)

#define B_ENUM_AS_FLAGS_EX(T, enumBasicType) \
inline T operator & (T x, T y) { return static_cast<T> (static_cast<enumBasicType>(x) & static_cast<enumBasicType>(y)); }; \
inline T operator | (T x, T y) { return static_cast<T> (static_cast<enumBasicType>(x) | static_cast<enumBasicType>(y)); }; \
inline T operator ^ (T x, T y) { return static_cast<T> (static_cast<enumBasicType>(x) ^ static_cast<enumBasicType>(y)); }; \
inline T operator ~ (T x) { return static_cast<T> (~static_cast<enumBasicType>(x)); }; \
inline T& operator &= (T& x, T y) { x = x & y; return x; }; \
inline T& operator |= (T& x, T y) { x = x | y; return x; }; \
inline T& operator ^= (T& x, T y) { x = x ^ y; return x; }; \
inline bool operator !(T x) { return !(enumBasicType)(x); }; \
inline bool checkFlags(T x) { return (enumBasicType)x != 0; }; \
inline bool any(T x) { return (enumBasicType)x != 0; }; \
inline bool has(T x, T y) { return ((enumBasicType)x & (enumBasicType)y) != 0; }; \
inline enumBasicType fromFlags(T x) { return (enumBasicType)x; };
template <typename T>
inline T toFlags(int x) { return (T)x; };