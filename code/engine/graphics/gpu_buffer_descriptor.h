// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/array.h"
#include "base/logger.h"
#include "graphics/types.h"

namespace engine
{
class E_API GpuBufferDescriptor
{
public:
	GpuBufferDescriptor()
		: totalSize(0)
	{}

	inline void clear()
	{
		elements.clear();
		totalSize = 0;
	}

	void addElement(const GpuBufferElement& element)
	{
		GpuBufferElement bufferElement = element;

		bufferElement.offset = totalSize;
		bufferElement.index = getElementIndex(element);
		bufferElement.updateComponentSize();
		elements.append(bufferElement);
		totalSize += bufferElement.getElementSize();
	}

	void addElement(GpuBufferElementComponentType compType, u32 count, GpuBufferElementType type, u32 index = 0)
	{
		GpuBufferElement element(compType, count, type);
		element.index = index;
		addElement(element);
	}

	inline void addPositionElement()
	{
		addElement(GpuBufferElementComponentType::Float, 3, GpuBufferElementType::Position);
	}

	inline void addNormalElement()
	{
		addElement(GpuBufferElementComponentType::Float, 3, GpuBufferElementType::Normal);
	}

	inline void addTangentElement()
	{
		addElement(GpuBufferElementComponentType::Float, 3, GpuBufferElementType::Tangent);
	}

	inline void addBitangentElement()
	{
		addElement(GpuBufferElementComponentType::Float, 3, GpuBufferElementType::Bitangent);
	}

	inline void addColorElement()
	{
		addElement(GpuBufferElementComponentType::Float, 4, GpuBufferElementType::Color);
	}

	inline void addTexCoordElement(u32 index = 0)
	{
		addElement(GpuBufferElementComponentType::Float, 2, GpuBufferElementType::TexCoord, index);
	}

	inline void addJointIndexElement(u32 index = 0)
	{
		addElement(GpuBufferElementComponentType::Int16, 1, GpuBufferElementType::JointIndex, index);
	}

	inline void addJointWeightElement(u32 index = 0)
	{
		addElement(GpuBufferElementComponentType::Float, 1, GpuBufferElementType::JointWeight, index);
	}

	inline void addIndexElement()
	{
		addElement(GpuBufferElementComponentType::Int32, 1, GpuBufferElementType::Index);
	}

	inline u32 getStride() const
	{
		return totalSize;
	}

	inline const Array<GpuBufferElement>& getElements() const
	{
		return elements;
	}

protected:
	u32 getElementIndex(const GpuBufferElement& element)
	{
		u32 index = 0;

		for (auto& el : elements)
		{
			if (el.type == element.type)
			{
				++index;
			}
		}

		return index;
	}

	Array<GpuBufferElement> elements;
	u32 totalSize;
};

}