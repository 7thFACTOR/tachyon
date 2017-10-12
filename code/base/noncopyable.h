// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once

namespace base
{
class NonCopyable
{
public:
	NonCopyable() {}

private:
	NonCopyable(const NonCopyable&) {}
	NonCopyable& operator = (const NonCopyable&) { return *this; }
};	

}