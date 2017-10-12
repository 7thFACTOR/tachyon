// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include <cstddef>
#include <cstdint>
#ifdef _LINUX
#include <pthread.h>
#endif

#include <cstddef>

using namespace std;


#ifndef B_NO_BASIC_TYPES

#ifndef B_NO_U8
typedef unsigned char u8;
#endif

#ifndef B_NO_U16
typedef unsigned short u16;
#endif

#ifndef B_NO_U32
typedef unsigned int u32;
#endif

#ifndef B_NO_U64
typedef unsigned long long u64;
#endif

#ifndef B_NO_U128
//! a 128 bit unsigned integer as a char array
#pragma pack(push, 1)
typedef struct u128_t
{
	u64 data[2];

	static const u128_t nullValue() { return{ 0, 0 }; };

	u128_t() {}
	u128_t(u64 a, u64 b)
	{
		data[0] = a;
		data[1] = b;
	}

	bool operator == (const u128_t& other) { return data[0] == other.data[0] && data[1] == other.data[1]; }
	bool operator != (const u128_t& other) { return data[0] != other.data[0] || data[1] != other.data[1]; }
	u128_t& operator = (const u128_t& other) { data[0] = other.data[0]; data[1] = other.data[1]; return *this; }
} u128;
#pragma pack(pop)
#endif

#ifndef B_NO_I8
typedef char i8;
#endif

#ifndef B_NO_I16
typedef short i16;
#endif

#ifndef B_NO_I32
typedef int i32;
#endif

#ifndef B_NO_I64
typedef long long i64;
#endif

#ifndef B_NO_I128
//! a 128 bit signed integer
typedef u128 i128;
#endif

#ifndef B_NO_F32
typedef float f32;
#endif

#ifndef B_NO_F64
typedef double f64;
#endif

#endif

namespace base
{
typedef u32 IconHandle;

#ifdef _WINDOWS
typedef u64 WindowHandle;
typedef u64 ThreadHandle;
typedef u64 ThreadId;
typedef void* LibraryHandle;
typedef void* CriticalSectionHandle;
#elif _LINUX
typedef u32 WindowHandle;
typedef pthread_t ThreadHandle;
typedef pthread_t ThreadId;
typedef void* LibraryHandle;
typedef pthread_mutex_t* CriticalSectionHandle;
#endif

typedef u32 IconId;
typedef u32 TimerId;
}

#include "base/memory.h"
