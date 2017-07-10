#include "gui/widget.h"
#include "gui/canvas.h"
#include "gui/event.h"
#include "base/qsort.h"
#include "base/util.h"

namespace engine
{
Widget::Widget()
{
	tabIndex = 0;
	flags.enabled = 1;
	flags.focusable = 1;
	flags.focused = 0;
	flags.captured = 0;
	flags.visible = 1;
}

Widget::~Widget()
{
	for (auto child : children)
	{
		delete child;
	}
}

void Widget::setTheme(Theme* theme)
{
	this->theme = theme;
}

void Widget::addChild(Widget* child)
{
	child->parent = this;
	children.appendUnique(child);
}

void Widget::removeChild(Widget* child)
{
	children.erase(child);
}

void Widget::updateWorldRect()
{
	if (!parent)
	{
		transform.localRect.set(transform.position.x, transform.position.y, transform.size.x, transform.size.y);
		transform.worldRect = transform.localRect;

		for (auto child : children)
		{
			child->updateWorldRect();
		}

		return;
	}

	transform.localRect.x =
		parent->transform.localRect.width * transform.anchorMin.x;
	transform.localRect.y =
		parent->transform.localRect.height * transform.anchorMin.y;
	transform.localRect.width =
		parent->transform.localRect.width * transform.anchorMax.x
		- transform.localRect.x;
	transform.localRect.height =
		parent->transform.localRect.height * transform.anchorMax.y
		- transform.localRect.y;

	transform.worldRect.x = parent->transform.worldRect.x + transform.localRect.x;
	transform.worldRect.y = parent->transform.worldRect.y + transform.localRect.y;
	transform.worldRect.width = transform.localRect.width;
	transform.worldRect.height = transform.localRect.height;

	for (auto child : children)
	{
		child->updateWorldRect();
	}
}

void Widget::updateLocalMatrix()
{
	localMatrix.setIdentity();

	if (!parent)
	{
		return;
	}

	Matrix rotationMtx;
	Matrix scaleMtx;
	Matrix pivotMtx;

	pivotMtx.setTranslation(
		-parent->transform.localRect.width * transform.pivot.x,
		-parent->transform.localRect.height * transform.pivot.y, 0);

	scaleMtx.setScale(transform.scale);
	rotationMtx.setRotation(transform.rotation.x, transform.rotation.y, transform.rotation.z);
	localMatrix = pivotMtx * scaleMtx * rotationMtx;
}

void Widget::updateWorldMatrix()
{
	if (parent)
	{
		worldMatrix = localMatrix * parent->worldMatrix;
	}
	else
	{
		worldMatrix.setIdentity();
	}

	for (auto child : children)
	{
		child->updateWorldMatrix();
	}
}

void Widget::updateInternal(f32 deltaTime)
{
	updateLocalMatrix();
	updateWorldMatrix();
	update(deltaTime);

	for (auto child : children)
	{
		child->updateInternal(deltaTime);
	}
}

void Widget::drawInternal(Canvas& canvas)
{
	draw(canvas);

	for (auto child : children)
	{
		child->drawInternal(canvas);
	}
}

void Widget::handleEventInternal(WidgetEvent& e)
{
	if (only2D)
	{
		e.point = e.windowPoint - toVec2(transform.position);
	}
	else
	{
		//TODO: 3D
	}

	handleEvent(e);

	// if the widget canceled the event propagation, end here
	if (e.cancelBubble)
	{
		return;
	}

	// bubble down event towards the bottom most root parent
	if (parent)
	{
		parent->handleEventInternal(e);
	}
}

bool sortByTabIndex(const Widget* a, const Widget* b)
{
	return a->getTabIndex() > b->getTabIndex();
}

Widget* Widget::handleChangeFocusInChildren(Widget* focusedWidget)
{
	if (!children.isEmpty())
	{
		focusedWidget = nextSibling(focusedWidget);
	}

	if (children.isEmpty() || !focusedWidget)
	{
		if (parent)
		{
			focusedWidget = parent->handleChangeFocusInChildren(this);
		}
	}

	if (!parent && !focusedWidget)
	{
		if (!children.isEmpty())
		{
			focusedWidget = children[0];
		}
		else
		{
			focusedWidget = this;
		}
	}

	return focusedWidget;
}

Widget* Widget::nextFocusableWidgetRecursive(Widget* widget)
{
	if (widget->flags.focusable)
	{
		return widget;
	}

	for (auto child : widget->children)
	{
		Widget* focused = nextFocusableWidgetRecursive(child);

		if (focused)
		{
			return focused;
		}
	}

	return nullptr;
}

Widget* Widget::nextSibling(Widget* focusedWidget)
{
	if (focusedWidget == this)
	{
		return children[0];
	}

	i32 index = 0;
	Widget* nextWidget = nullptr;
	Array<Widget*> sortedChildren = children;

	//TODO:
	//B_QSORT(Widget, sortedChildren.begin(), sortedChildren.size(), sortByTabIndex);

	for (auto child : sortedChildren)
	{
		if (focusedWidget == child)
		{
			break;
		}

		index++;
	}

	// increment to next child
	index++;

	// if end of children, go to parent
	if (index >= sortedChildren.size())
	{
		if (parent)
		{
			nextWidget = parent->handleChangeFocusInChildren(this);
		}
		else
		{
			nextWidget = sortedChildren[0];
		}
	}
	else
	{
		// we can advance to the next child
		// jump to next focusable child
		while (index < sortedChildren.size())
		{
			Widget* widget = nextFocusableWidgetRecursive(sortedChildren[index]);

			if (widget)
			{
				nextWidget = widget;
				break;
			}

			index++;
		}

		// if we don't have any focusable child, then ask parent
		if (index >= sortedChildren.size())
		{
			if (parent)
			{
				nextWidget = parent->handleChangeFocusInChildren(this);
			}
			else
			{
				nextWidget = sortedChildren[0];
			}
		}
	}

	return nextWidget;
}

void Widget::handleEvent(WidgetEvent& e)
{
	switch (e.type)
	{
	case WidgetEventType::MouseButtonDown:
		if (e.button == MouseButton::Left)
		{
			flags.pressed = 1;
			flags.captured = 1;
		}
		break;
	case WidgetEventType::MouseButtonUp:
		if (e.button == MouseButton::Left)
		{
			if (isPointInWidget(e.windowPoint))
			{
				if (actionCallback)
				{
					actionCallback(this, e, WidgetActionType::Clicked);
				}
			}

			flags.pressed = 0;
			flags.captured = 0;
		}
		break;
	default:
		break;
	}
}

bool Widget::isPointInWidget(const Vec2& screenPt)
{
	if (only2D)
	{
		return transform.worldRect.contains(screenPt);
	}

	//TODO: 3D ray check

	return false;
}

}