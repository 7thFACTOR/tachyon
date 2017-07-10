#pragma once
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "core/defines.h"
#include "resources/gpu_program_resource.h"

namespace engine
{
using namespace base;

//! A gpu program constant, its value it is the same for all rendered meshes\n
class ENGINE_API GpuProgramConstant
{
public:
	GpuProgramConstant();
	GpuProgramConstant(const GpuProgramConstant& other);
	virtual ~GpuProgramConstant();

	//! setup a constant
	//! \param aType the standard type
	//! \param pName the name as it is defined in the HLSL/GLSL file
	//! \param pData the pointer to the constant variable; it will read each frame
	//! the value from there, so don't destroy that variable,
	//! until this class is not destroyed
	//! \param bIsArray is this constant a constant array ?
	//! \param aCount if its an array then specify the count
	void set(
		GpuProgramConstantType type,
		const String& name,
		void* data = nullptr,
		u32 count = 1);

	//! set the constant value(s), WARNING: you must know the type of this constant and provide the right data types
	void setDataValue(void* data, u32 count = 1);
	void* getData() const { return data; }
	const String& getName() const { return name; }
	GpuProgramConstantType getType() const { return type; }
	u32 getCount() const { return count; }
	u32 getDataSize() const { return dataSize; }

	GpuProgramConstant& operator = (const GpuProgramConstant& other);

protected:
	void createData();
	void freeData();

	//! the constant type
	GpuProgramConstantType type;
	//! the constant name
	String name;
	//! if this a value array, this is the count of elements
	u32 count;
	//! the actual constant data address
	void* data;
	u32 dataSize;
};

}