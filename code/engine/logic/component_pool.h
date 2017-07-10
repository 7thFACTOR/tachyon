#pragma once
#include "base/defines.h"
#include "base/types.h"
#include "base/string.h"
#include "base/array.h"
#include "base/assert.h"
#include "logic/types.h"
#include "logic/entity.h"

namespace engine
{
using namespace base;

struct Component;
class Entity;

//! The component pool holds contiguous arrays of Component derived structures
class ComponentPool
{
public:
	virtual ~ComponentPool(){}
	virtual inline ComponentTypeId getComponentTypeId() const = 0;
	virtual Component* createComponent() = 0;
	virtual bool deleteComponent(Component* component) = 0;
	virtual inline size_t getComponentCount() const = 0;
	virtual inline Component* getComponentAt(size_t index) = 0;
	template <typename ComponentStructType>
	inline ComponentStructType* getComponentAs(size_t index) { return (ComponentStructType*)getComponentAt(index); }
	virtual void deleteAllComponents() = 0;
	virtual void deleteEntityComponents(Entity* entity) = 0;
	virtual void clearDirtyFlag() = 0;
	virtual bool isAllowingMultipleComponentsPerEntity() const = 0;
};

template<typename ComponentStructType, bool doAllowMultiplePerEntity = false>
class ComponentPoolTpl : public ComponentPool
{
public:
	struct ComponentArray
	{
		Array<ComponentStructType> components;
		Array<size_t> freeComponentIndices;
	};

	ComponentPoolTpl()
	{}

	~ComponentPoolTpl()
	{
		for (size_t i = 0; i < componentArrays.size(); i++)
		{
			delete componentArrays[i];
		}

		componentArrays.clear();
	}
	
	inline ComponentTypeId getComponentTypeId() const override
	{
		return (ComponentTypeId)ComponentStructType::typeId;
	}

	Component* createComponent() override
	{
		Component* comp = nullptr;
		size_t foundArrayIndex = ~0;

		for (size_t i = 0; i < componentArrays.size(); i++)
		{
			if (!componentArrays[i]->freeComponentIndices.isEmpty())
			{
				foundArrayIndex = i;
			}
		}

		if (foundArrayIndex == ~0)
		{
			// we need a new array
			addComponentArray();
			foundArrayIndex = componentArrays.size() - 1;
		}

		size_t freeIndex;

		componentArrays[foundArrayIndex]->freeComponentIndices.popLast(freeIndex);
		comp = &componentArrays[foundArrayIndex]->components[freeIndex];

		// init
		*(ComponentStructType*)comp = ComponentStructType();
		comp->type = ComponentStructType::typeId;
		comp->used = true;
		comp->dirty = true;

		componentCount++;

		return comp;
	}
	
	bool deleteComponent(Component* comp) override
	{
		B_ASSERT(comp);
		comp->used = false;

		// find its array
		for (size_t i = 0; i < componentArrays.size(); i++)
		{
			if ((u64)comp >= (u64)componentArrays[i]->components.data()
				&& (u64)comp < (u64)componentArrays[i]->components.data() + (u64)componentArrays[i]->components.byteSize())
			{
				size_t index = ((u64)comp - (u64)componentArrays[i]->components.data()) / sizeof(ComponentStructType);
				componentArrays[i]->freeComponentIndices.append(index);
				componentCount--;

				return true;
			}
		}

		return false;
	}
	
	inline size_t getComponentCount() const override
	{
		return componentCount;
	}

	inline Component* getComponentAt(size_t index) override
	{
		size_t indexInsideArray = index % componentCountPerArray;
		size_t arrayIndex = index / componentCount;
		
		return (Component*)&componentArrays[arrayIndex]->components[indexInsideArray];
	}

	void clearDirtyFlag() override
	{
		for (auto componentArray : componentArrays)
		{
			for (auto& comp : componentArray->components)
			{
				comp.dirty = false;
			}
		}
	}

	void deleteAllComponents() override
	{
		for (auto componentArray : componentArrays)
		{
			for (auto& comp : componentArray->components)
			{
				comp.used = false;
			}
		}
	}

	void deleteEntityComponents(Entity* entity) override
	{
		for (size_t i = 0; i < entity->getComponents().size(); i++)
		{
			deleteComponent(entity->getComponents().valueAt(i));
		}
	}

	bool isAllowingMultipleComponentsPerEntity() const override { return allowMultiplePerEntity; }

protected:
	void addComponentArray()
	{
		ComponentArray* arr = new ComponentArray();

		B_ASSERT(arr);
		if (arr)
		{
			arr->components.resize(componentCountPerArray);
			arr->freeComponentIndices.resize(componentCountPerArray);

			for (size_t i = 0; i < componentCountPerArray; i++)
			{
				arr->freeComponentIndices[i] = componentCountPerArray - i - 1;
			}

			componentArrays.append(arr);
		}
	}

	size_t componentCount = 0;
	bool allowMultiplePerEntity = doAllowMultiplePerEntity;
	Array<ComponentArray*> componentArrays;
};

}