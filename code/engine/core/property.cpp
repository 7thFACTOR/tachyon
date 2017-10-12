// Copyright (C) 2017 7thFACTOR Software, All rights reserved
#include "core/property.h"
#include "logic/entity.h"
#include "base/assert.h"

namespace engine
{
PropertyType getPropertyType(i8* member) { return PropertyType::Int8; }
PropertyType getPropertyType(i16* member) { return PropertyType::Int16; }
PropertyType getPropertyType(i32* member) { return PropertyType::Int32; }
PropertyType getPropertyType(i64* member) { return PropertyType::Int64; }
PropertyType getPropertyType(u8* member) { return PropertyType::Uint8; }
PropertyType getPropertyType(u16* member) { return PropertyType::Uint16; }
PropertyType getPropertyType(u32* member) { return PropertyType::Uint32; }
PropertyType getPropertyType(u64* member) { return PropertyType::Uint64; }
PropertyType getPropertyType(f32* member) { return PropertyType::Float; }
PropertyType getPropertyType(f64* member) { return PropertyType::Double; }
PropertyType getPropertyType(String* member) { return PropertyType::String; }
PropertyType getPropertyType(bool* member) { return PropertyType::Bool; }
PropertyType getPropertyType(Vec2* member) { return PropertyType::Vec2; }
PropertyType getPropertyType(Vec3* member) { return PropertyType::Vec3; }
PropertyType getPropertyType(BigVec3* member) { return PropertyType::BigVec3; }
PropertyType getPropertyType(Color* member) { return PropertyType::Color; }
PropertyType getPropertyType(Quat* member) { return PropertyType::Quat; }
PropertyType getPropertyType(Matrix* member) { return PropertyType::Matrix; }
PropertyType getPropertyType(Entity* member) { return PropertyType::Entity; }
PropertyType getPropertyType(Component* member) { return PropertyType::Component; }

//TODO:
void PropertyHolder::setPropertyValue(const String& name, const Variant& value)
{
	B_ASSERT_NOT_IMPLEMENTED
}

void PropertyHolder::setPropertyValue(const String& name, const String& value)
{
	B_ASSERT_NOT_IMPLEMENTED
}

void PropertyHolder::resetPropertyValue(const String& name)
{
	B_ASSERT_NOT_IMPLEMENTED
}

Variant PropertyHolder::getPropertyValue(const String& name)
{
	B_ASSERT_NOT_IMPLEMENTED
	return Variant();
}

String PropertyHolder::getPropertyValueAsText(const String& name)
{
	B_ASSERT_NOT_IMPLEMENTED
	return "";
}

void PropertyHolder::serialize(Stream& stream)
{
	B_ASSERT_NOT_IMPLEMENTED
}

void PropertyHolder::deserialize(Stream& stream)
{
	B_ASSERT_NOT_IMPLEMENTED
}

}