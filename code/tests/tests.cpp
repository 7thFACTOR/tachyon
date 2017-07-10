#include "core/globals.h"
#include "core/module.h"
#include "base/variable.h"
#include "base/json.h"
#include "base/logger.h"
#include "core/resources/texture_resource.h"
#include "input/input_map.h"
#include "base/qsort.h"
#include "base/stdio_logger.h"
#include "core/property.h"
#include "base/misc/cached_font.h"
#include "base/misc/image_atlas.h"
#include "base/file.h"

#include "logic/component.h"
#include "logic/entity.h"
#include "logic/world.h"
#include "logic/logic.h"

using namespace engine;
using namespace base;

Dictionary<int, String> types;

void printJsonValue(JsonValue& value)
{
	World w;
	Logic g;
	
	auto e = w.createEntity();

	//e->addComponent(g.createComponent(StdComponentTypeId_AmbientMusic));

	w.deleteEntity(e);

	B_LOG_INFO((value.isArray() ? "Array" : "Object") << ":");
	
	if (value.isArray())
	{
		B_LOG_INFO(value.asArray()->size() << " array elements");
		B_LOG_INDENT;

		for (size_t i = 0; i < value.asArray()->size(); ++i)
		{
			auto val = value.asArray()->at(i);

			if (val->isSimpleType())
			{
				B_LOG_INFO("Simple array value: '" << val->toString() << "' Type: " << types[(int)val->getType()]);
			}
			else
			{
				B_LOG_INFO("Node array value:");
				printJsonValue(*val);
			}
		}

		B_LOG_UNINDENT;
	}
	else
	{
		auto obj = value.asObject();
		B_LOG_INFO(obj->getMembers().size() << " name-value pairs");
		B_LOG_INDENT;

		for (auto iter = obj->getMembers().begin(), iterEnd = obj->getMembers().end(); iter != iterEnd; ++iter)
		{
			auto val = iter->value;

			if (val->isSimpleType())
			{
				B_LOG_INFO("Name: '" << iter->key << "' Value: '" << val->toString() << "' Type: " << types[(int)val->getType()]);
			}
			else
			{
				B_LOG_INFO("Name: '" << iter->key << "' Type: " << (val->isArray() ? "Array" : "Object"));
				printJsonValue(*val);
			}
		}

		B_LOG_UNINDENT;
	}
}

bool funclt(int* el1, int* el2)
{
	return *el1 < *el2;
}

void debugProperties(Component* comp)
{
	B_LOG_INFO("Properties of component type: %u", comp->type);
}

struct PropTestClass : public PropertyHolder
{
	int m1 = 2;
	float m2 = 111;
	String m3 = "test";

	E_HAS_PROPERTIES
};

enum class TestEnum
{
	One,
	Two,
	Three
};

E_BEGIN_ENUM(TestEnum)
	E_ENUM(One)
	E_ENUM(Two)
	E_ENUM(Three)
E_END_ENUM(TestEnum)

struct PropTestClass2 : public PropTestClass
{
	int m4 = 2111;
	float m5 = 333;
	String m6 = "test2222";
	TestEnum enu;

	E_HAS_PROPERTIES
};

E_BEGIN_PROPERTIES(PropTestClass)
	E_PROPERTY(m1, "", 0, PropertyFlags::None)
	E_PROPERTY(m2, "", 0, PropertyFlags::None)
	E_PROPERTY(m3, "", 0, PropertyFlags::None)
E_END_PROPERTIES

E_BEGIN_PROPERTIES_INHERITED(PropTestClass2, PropTestClass)
	E_PROPERTY(m4, "", 0, PropertyFlags::None)
	E_PROPERTY(m5, "", 0, PropertyFlags::None)
	E_PROPERTY_GROUP("Group1", "Some group here")
	E_PROPERTY(m6, "", 0, PropertyFlags::None)
	E_PROPERTY_ENUM(enu, TestEnum, "", TestEnum::One, PropertyFlags::None)
E_END_PROPERTIES

int main(int argc, char** args)
{
	StdioLogger logger;
	base::getBaseLogger().linkChild(&logger);

	PropTestClass props;
	PropTestClass2 props2;

	B_LOG_INFO("Prop count: " << props.getPropertyCount());
	B_LOG_INFO("Prop2 count: " << props2.getPropertyCount());

	for (size_t i = 0; i < props2.getPropertyCount(); i++)
	{
		Property prop;

		props2.queryProperty(i, prop);

		B_LOG_INFO("\tProp: '" << prop.name << "' at " << i << ", type " << (u32)prop.type << ", desc: " << prop.description << " addr: " << (u64)prop.member);
	}

	B_LOG_INFO("Enum count: " << E_GET_ENUM_COUNT(TestEnum));

	for (u32 i = 0; i < E_GET_ENUM_COUNT(TestEnum); i++)
	{
		auto info = E_GET_ENUM_INFO(TestEnum, i);
		B_LOG_INFO("\tEnum: '" << info.name << "' Value: " << info.value);
	}

	CachedFont fnt("../assets/engine/fonts/arial.ttf", 38);

	fnt.precacheLatinAlphabetGlyphs();
	fnt.getGlyph(0x98a2);

	File fntAtlasImgFile;

	fntAtlasImgFile.open("font_atlas.raw", FileOpenFlags::BinaryWrite);
	fntAtlasImgFile.write(fnt.getAtlas().getAtlasImageBuffer(), fnt.getAtlas().getAtlasImageBufferSize());
	fntAtlasImgFile.close();
	
	JsonDocument doc;
	
	types[(int)JsonValueType::Null] = "Null";
	types[(int)JsonValueType::Float] = "Float";
	types[(int)JsonValueType::Integer] = "Int";
	types[(int)JsonValueType::Bool] = "Bool";
	types[(int)JsonValueType::String] = "String";

	bool result = doc.loadAndParse("../data_raw/test.json");

	if (result)
	{
		B_LOG_INFO("Success!");
		JsonValue val;

		val.assignValue(JsonValueType::Object, &doc.getRoot());
		printJsonValue(val);
		val.assignValue(JsonValueType::Null, 0);
	}

	doc.beginDocument();

	doc.addValue("shit1", 111);
	doc.addValue("shit2", 123.23f);
	doc.addValue("shit3", false);
	doc.addValue("shit4", "CRAPO");
	doc.beginArray("shit5");
		doc.addValue(1);
		doc.addValue(2);
		doc.addValue(3.33f);
		doc.beginObject();
			doc.addValue("w", 11);
			doc.addValue("h", 22);
		doc.endObject();
	doc.endArray();
	doc.beginObject("obo nobo");
		doc.addValue("width", 11111);
		doc.addValue("height", 22222);
		doc.beginObject("soko");
			doc.addValue("ass", 1001.5f);
		doc.endObject();
	doc.endObject();
	doc.endDocument();
	doc.save("testwrite.json", &JsonSaveOptions());
	doc.save("testwrite.sjson", &JsonSaveOptions::sjsonOptions());

	B_LOG_INFO(doc.getF32("obo/soko/ass"));
	B_LOG_INFO(doc.getF32("obo/soko/asds"));
	B_LOG_INFO(doc.getF32(""));

	Array<Array<int> > a;

	a.resize(3);
	a[0].append(1);
	a[0].append(2);
	a[0].append(3);
	a.clear();

	InputMap im;

	im.load("../config/game.input");
	im.save("../config/game2.input");

	Array<int> sa;

	sa.append(4);
	sa.append(2);
	sa.append(1);
	sa.append(3);
	sa.append(5);

	B_QSORT(int, &sa[0], sa.size(), funclt);

	for (int i = 0; i < sa.size(); ++i)
	{
		B_LOG_INFO("EL: " << sa.at(i));
	}

	for (auto el : sa)
	{
		B_LOG_INFO("RANGE BASED EL: " << el);
	}

	return 0;
}
