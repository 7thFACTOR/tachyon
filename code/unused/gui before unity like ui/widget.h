#pragma once
#include "base/array.h"
#include "base/string.h"
#include "core/types.h"
#include "core/defines.h"
#include "base/math/rect.h"
#include "base/math/vec2.h"
#include "base/math/vec3.h"
#include "base/math/matrix.h"
#include "gui/layout.h"
#include "gui/event.h"

namespace engine
{
struct Theme;
struct WidgetEvent;
class Canvas;

enum class WidgetActionType
{
	Unknown,
	Clicked,
	Changed,
	
	Count
};

union WidgetFlags
{
	WidgetFlags()
	{
		flags = 0;
		visible = 1;
		enabled = 1;
	}
	
	struct
	{
		u32 visible : 1;
		u32 enabled : 1;
		u32 hovered : 1;
		u32 pressed : 1;
		u32 captured : 1;
		u32 focused : 1;
		u32 focusable : 1;
	};

	u32 flags = 0;
};

struct WidgetTransform
{
	WidgetTransform() {}
	WidgetTransform(
		const Vec3& Position, const Vec2& Size,
		const Vec2& AnchorMin = Vec2(0.5f, 0.5f),
		const Vec2& AnchorMax = Vec2(0.5f, 0.5f),
		const Vec2& Pivot = Vec2(0.5f, 0.5f),
		const Vec3& Rotation = Vec3(),
		const Vec3& Scale = Vec3(1.0f, 1.0f, 1.0f))
		: position(Position)
		, size(Size)
	{}

	Vec3 position;
	Vec2 size;
	Vec2 anchorMin, anchorMax;
	Vec2 pivot;
	Vec3 rotation;
	Vec3 scale;
	Rect localRect;
	Rect worldRect;
};

typedef void (*WidgetActionCallback)(Widget* widget, WidgetEvent& event, WidgetActionType action);

class ENGINE_API Widget
{
public:
	Widget();
	Widget(const String& initialText, const WidgetTransform& initialTransform)
		: text(initialText)
		, transform(initialTransform)
	{}
	virtual ~Widget();

	void setActionCallback(WidgetActionCallback callback) { actionCallback = callback; }
	virtual void setText(const String& value) { text = value; }
	void setTransform(const WidgetTransform& value) { transform = value; }
	void setPosition(const Vec3& value) { transform.position = value; }
	void setSize(const Vec2& value) { transform.size = value; }
	void setRect(const Rect& rect)
	{
		transform.position.set(rect.x, rect.y, transform.position.z);
		transform.size.set(rect.width, rect.height);
	}
	void setAnchors(const Rect& rect)
	{
		transform.anchorMin = rect.topLeft();
		transform.anchorMax = rect.bottomRight();
	}

	void show() { flags.visible = 1; }
	void hide() { flags.visible = 0; }
	void setTheme(Theme* theme);
	Theme* getTheme() const { return theme; }
	const WidgetTransform& getTransform() const { return transform; }
	void addChild(Widget* child);
	void removeChild(Widget* child);
	Layout& getLayout() { return layout; }
	const String& getText() const { return text; }
	u32 getTabIndex() const { return tabIndex; }
	bool isPointInWidget(const Vec2& screenPt);

	virtual void handleEvent(WidgetEvent& e);
	virtual void update(f32 deltaTime) {}
	virtual void draw(Canvas& canvas) {}

protected:
	void updateWorldRect();
	void updateLocalMatrix();
	void updateWorldMatrix();
	void updateInternal(f32 deltaTime);
	void drawInternal(Canvas& canvas);
	void handleEventInternal(WidgetEvent& e);
	Widget* handleChangeFocusInChildren(Widget* focusedWidget);
	Widget* nextSibling(Widget* focusedWidget);
	Widget* nextFocusableWidgetRecursive(Widget* widget);

	bool only2D = true;
	u32 tabIndex = 0;
	Layout layout;
	WidgetTransform transform;
	Matrix localMatrix;
	Matrix worldMatrix;
	Theme* theme = nullptr;
	WidgetFlags flags;
	String text;
	Widget* parent = nullptr;
	Array<Widget*> children;
	WidgetActionCallback actionCallback = nullptr;
};

}