#pragma once
#include "base/types.h"
#include "base/defines.h"
#include "base/string.h"

#ifdef _WINDOWS
#include <crtdbg.h>
#endif

#ifdef _LINUX
#include <signal.h>
#endif

#define B_STATIC_ASSERT(pred) switch(0) { case 0: case pred: ; }
#define B_CURRENT_COMPILE_LINE_AS_TEXT(line) #line
#define B_CURRENT_COMPILE_LOCATION __FILE__ ":" B_CURRENT_COMPILE_LINE_AS_TEXT(__LINE__)

#ifdef _DEBUG
	#define B_ASSERT(expr)\
		if (!(expr)) { if(::base::assertMessage(__FILE__, __FUNCTION__, __LINE__, ""#expr, "")) B_DEBUG_BREAK; }
	#define B_ASSERT_MSG(expr, msg)\
		if (!(expr)) { if(::base::assertMessage(__FILE__, __FUNCTION__, __LINE__, ""#expr, msg)) B_DEBUG_BREAK; }
	#define B_ASSERT_NOT_IMPLEMENTED B_ASSERT(!"Feature/code not yet implemented.")
#else
	#define B_ASSERT(expr)
	#define B_ASSERT_MSG(expr, msg)
	#define B_ASSERT_NOT_IMPLEMENTED
#endif

// these asserts will work on release mode also.
#define B_RELEASE_ASSERT(expr)\
		if (!(expr)) { if(::base::assertMessage(__FILE__, __FUNCTION__, __LINE__, ""#expr, "")) B_DEBUG_BREAK; }
#define B_RELEASE_ASSERT_MSG(expr, msg)\
		if (!(expr)) { if(::base::assertMessage(__FILE__, __FUNCTION__, __LINE__, ""#expr, msg)) B_DEBUG_BREAK; }
#define B_RELEASE_ASSERT_NOT_IMPLEMENTED B_RELEASE_ASSERT(!"Code not implemented yet.")

namespace base
{
//! \return true if the user wants to debug
B_EXPORT bool assertMessage(
	const String& filename,
	const String& function,
	u32 line,
	const String& expression,
	const String& message);

static void staticTypeSizeAssert()
{
	B_STATIC_ASSERT(sizeof(u8) == 1);
	B_STATIC_ASSERT(sizeof(u16) == 2);
	B_STATIC_ASSERT(sizeof(u32) == 4);
	B_STATIC_ASSERT(sizeof(u64) == 8);
	B_STATIC_ASSERT(sizeof(i8) == 1);
	B_STATIC_ASSERT(sizeof(i16) == 2);
	B_STATIC_ASSERT(sizeof(i32) == 4);
	B_STATIC_ASSERT(sizeof(i64) == 8);
	B_STATIC_ASSERT(sizeof(f32) == 4);
	B_STATIC_ASSERT(sizeof(f64) == 8);
};
}