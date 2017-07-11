#pragma once
#include "base/array.h"
#include "base/dictionary.h"
#include "base/string.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/variant.h"
#include "base/math/vec2.h"
#include "base/math/vec3.h"
#include "base/math/color.h"
#include "core/defines.h"
#include "core/types.h"

namespace engine
{
using namespace base;

class Entity;
struct Component;

enum class PropertyType
{
	None,
	Int8,
	Int16,
	Int32,
	Int64,
	Uint8,
	Uint16,
	Uint32,
	Uint64,
	Float,
	Double,
	String,
	Bool,
	Vec2,
	Vec3,
	BigVec3,
	Color,
	Quat,
	Matrix,
	Enum,
	Entity,
	Component,
	ResourceId,
	Array,
	Group,

	Count
};

template <typename Type> PropertyType getPropertyType(Type* member)
{
	return PropertyType::None;
}

E_API_EXPORT PropertyType getPropertyType(i8* member);
E_API_EXPORT PropertyType getPropertyType(i16* member);
E_API_EXPORT PropertyType getPropertyType(i32* member);
E_API_EXPORT PropertyType getPropertyType(i64* member);
E_API_EXPORT PropertyType getPropertyType(u8* member);
E_API_EXPORT PropertyType getPropertyType(u16* member);
E_API_EXPORT PropertyType getPropertyType(u32* member);
E_API_EXPORT PropertyType getPropertyType(u64* member);
E_API_EXPORT PropertyType getPropertyType(f32* member);
E_API_EXPORT PropertyType getPropertyType(f64* member);
E_API_EXPORT PropertyType getPropertyType(String* member);
E_API_EXPORT PropertyType getPropertyType(bool* member);
E_API_EXPORT PropertyType getPropertyType(Vec2* member);
E_API_EXPORT PropertyType getPropertyType(Vec3* member);
E_API_EXPORT PropertyType getPropertyType(BigVec3* member);
E_API_EXPORT PropertyType getPropertyType(Color* member);
E_API_EXPORT PropertyType getPropertyType(Quat* member);
E_API_EXPORT PropertyType getPropertyType(Matrix* member);
E_API_EXPORT PropertyType getPropertyType(Entity* member);
E_API_EXPORT PropertyType getPropertyType(Component* member);

enum class PropertyFlags
{
	None,
	//! cannot be modified in editor or console, but can be modified in code
	ReadOnly = B_BIT(0),
	//! the property will not be visible in editor or any other tool
	HiddenInEditor = B_BIT(1),
	//! the property will not be serialized
	NoSerialize = B_BIT(2),
	//! do not format name for display, otherwise editor will capitalize name and insert spaces
	//! 'someMember' will become 'Some Member', so this flag will keep the original name 'someMember'
	NoNameFormat = B_BIT(3),
	//! when playing in editor, this property will be disabled, not editable
	NoEditWhenPlaying = B_BIT(4),
	//! the property can be animated
	Animatable = B_BIT(5),
	//! the property value must use the range to limit values
	UseRange = B_BIT(6),
	//! if set, then this property cannot be upwards promoted to a prefab public property list
	NoPrefabPromote = B_BIT(7),
	//! if set, then the property is shown in the editor as a slider with percentage from 0-100%
	//! this works for int (0-100 range) and float/double (0-1 range) property types
	PercentageValue = B_BIT(8)
};
B_ENUM_AS_FLAGS(PropertyFlags);

struct Property
{
	PropertyType type = PropertyType::None;
	PropertyFlags flags = PropertyFlags::None;
	String name;
	String description;
	Variant defaultValue, minValue, maxValue;
	void* owner = nullptr;
	void* member = nullptr;
	size_t index = 0;
	ResourceType resourceType = ResourceType::None;
	struct EnumDescriptor* enumDescriptor = nullptr;
};

//! used as value for index on queryProperty to get property count in PropertyQuery.propertyIndex
const size_t queryPropertyCount = ~0;
//! should be enough for any class
const size_t maxPropertyCount = 1000;

class E_API PropertyHolder : public Serializable
{
public:
	PropertyHolder() {}
	virtual ~PropertyHolder() {}
	virtual bool queryProperty(size_t index, Property& prop) { return false; }
	void setPropertyValue(const String& name, const Variant& value);
	void setPropertyValue(const String& name, const String& value);
	void resetPropertyValue(const String& name);
	Variant getPropertyValue(const String& name);
	String getPropertyValueAsText(const String& name);
	void serialize(Stream& stream) override;
	void deserialize(Stream& stream) override;

	size_t getPropertyCount()
	{
		if (~0 == cachedPropertyCount)
			cachedPropertyCount = computePropertyCount();

		return cachedPropertyCount;
	}

protected:
	size_t computePropertyCount()
	{
		Property propQuery;

		if (queryProperty(queryPropertyCount, propQuery)
			&& propQuery.index < maxPropertyCount)
		{
			return propQuery.index;
		}

		return 0;
	}

	u32 cachedPropertyCount = ~0;
};

struct EnumDescriptor
{
	struct EnumInfo
	{
		String name;
		u32 value = 0;
		bool lastEnum = false;
	};

	virtual EnumInfo getEnumInfo(u32 index) = 0;
	u32 getEnumCount()
	{
		u32 count = 0;
		u32 i = 0;

		while (!getEnumInfo(i++).lastEnum)
		{
			count++;
		}

		return count;
	}
};

#define E_BEGIN_ENUM(enumType)\
struct enumType##Descriptor : EnumDescriptor\
{\
	typedef enumType EnumType;\
	EnumInfo getEnumInfo(u32 index) override\
	{\
	int currentIndex = 0; \
	EnumInfo info;

#define E_ENUM(enumName)\
	if (currentIndex == index)\
	{\
		info.name = B_STRINGIFY(enumName);\
		info.value = (u32)EnumType::enumName;\
		return info; \
	} currentIndex++;

#define E_END_ENUM(enumType)\
	info.lastEnum = true; \
	return info; \
	} } enumType##DescriptorVariable;

#define E_GET_ENUM_COUNT(enumType) enumType##DescriptorVariable.getEnumCount()
#define E_GET_ENUM_INFO(enumType, index) enumType##DescriptorVariable.getEnumInfo(index)

#define E_HAS_PROPERTIES\
	virtual bool queryProperty(size_t index, Property& prop) override;

#define E_BEGIN_PROPERTIES(className)\
	bool className::queryProperty(size_t index, Property& prop)\
	{\
		size_t currentIndex = 0;\
		bool found = false;

#define E_BEGIN_PROPERTIES_INHERITED(className, parentClassName)\
	bool className::queryProperty(size_t index, Property& prop)\
	{\
		size_t currentIndex = 0;\
		bool found = false;\
		found = parentClassName::queryProperty(index, prop);\
		if (found && index != queryPropertyCount)\
			{\
			return true;\
			}\
		currentIndex = prop.index;\

#define	E_PROPERTY(memberName, propDesc, propDefaultValue, propFlags)\
	if (currentIndex == index)\
			{\
		prop.type = getPropertyType(&memberName);\
		prop.name = #memberName;\
		prop.description = #propDesc;\
		prop.flags = propFlags;\
		prop.defaultValue = propDefaultValue;\
		prop.owner = this;\
		prop.member = &memberName;\
		prop.index = index;\
	} currentIndex++;

#define	E_PROPERTY_RANGED(memberName, propDesc, propDefaultValue, propMinValue, propMaxValue, propFlags)\
	if (currentIndex == index)\
			{\
		prop.type = getPropertyType(&memberName);\
		prop.name = #memberName;\
		prop.description = #propDesc;\
		prop.flags = propFlags;\
		prop.defaultValue = propDefaultValue;\
		prop.minValue = propMinValue;\
		prop.maxValue = propMaxValue;\
		prop.owner = this;\
		prop.member = &memberName;\
		prop.index = index;\
	} currentIndex++;

#define E_PROPERTY_RESOURCE(memberName, resourceType, propDesc, propDefaultValue, propFlags)\
	if (currentIndex == index)\
	{\
		prop.type = PropertyType::ResourceId;\
		prop.resourceType = resourceType;\
		prop.name = #memberName;\
		prop.description = #propDesc;\
		prop.flags = propFlags;\
		prop.defaultValue = propDefaultValue;\
		prop.owner = this;\
		prop.member = &memberName;\
		prop.index = index;\
	} currentIndex++;

#define	E_PROPERTY_ARRAY(memberName, arrayElementsType, propDesc, propFlags)\
	if (currentIndex == index)\
	{\
		prop.type = PropertyType::Array;\
		prop.arrayElementsType = arrayElementsType;\
		prop.name = #memberName;\
		prop.description = #propDesc;\
		prop.flags = propFlags;\
		prop.owner = this;\
		prop.member = &memberName;\
		prop.index = index;\
	} currentIndex++;

#define	E_PROPERTY_ENUM(memberName, enumType, propDesc, propDefaultValue, propFlags)\
	if (currentIndex == index)\
	{\
		prop.type = PropertyType::Enum;\
		prop.name = #memberName;\
		prop.description = #propDesc;\
		prop.flags = propFlags;\
		prop.owner = this;\
		prop.member = &memberName;\
		prop.enumDescriptor = &enumType##DescriptorVariable;\
		prop.index = index;\
	} currentIndex++;


#define	E_PROPERTY_GROUP(groupName, groupDesc)\
	if (currentIndex == index)\
	{\
		prop.type = PropertyType::Group;\
		prop.name = #groupName;\
		prop.description = #groupDesc;\
		prop.owner = this;\
		prop.index = index;\
	} currentIndex++;

#define E_END_PROPERTIES\
	prop.index = currentIndex;\
	if (index == queryPropertyCount)\
	{\
		return true;\
	}\
	return false; }

}