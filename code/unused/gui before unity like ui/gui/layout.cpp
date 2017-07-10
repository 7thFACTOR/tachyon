#include "gui/layout.h"
#include "gui/widget.h"

namespace engine
{
void Layout::arrangeWidgets(const Array<Widget*>& widgets)
{
	//if (type == LayoutType::None)
	//{
	//	return;
	//}

	//if (widgets.empty())
	//{
	//	return;
	//}

	//f32 w = 0.0f;
	//f32 h = 0.0f;
	//Rect rc;
	//f32 x, y;

	//switch (type)
	//{
	//case horus::LayoutType::Vertical:
	//	h = height / widgets.size() - spacing * (widgets.size() - 1);
	//	y = 0.0f;
	//	
	//	for (auto widget : widgets)
	//	{
	//		if (sizingPolicy == LayoutSizingPolicy::Keep)
	//		{
	//			rc.set(
	//				(width - widget->getRect().width) / 2.0f,
	//				y + (height - widget->getRect().height) / 2.0f,
	//				widget->getRect().width, widget->getRect().height);
	//		}
	//		else
	//		{
	//			rc.set(0.0f, y, width, h);
	//		}
	//		
	//		widget->setRect(rc);
	//		y += h + spacing;
	//	}

	//	break;
	//case horus::LayoutType::Horizontal:
	//	w = width / widgets.size() - spacing * (widgets.size() - 1);
	//	x = 0.0f;
	//	
	//	for (auto widget : widgets)
	//	{
	//		if (sizingPolicy == LayoutSizingPolicy::Keep)
	//		{
	//			rc.set(
	//				x + (w - widget->getRect().width) / 2.0f,
	//				(height - widget->getRect().height) / 2.0f,
	//				widget->getRect().width, widget->getRect().height);
	//		}
	//		else
	//		{
	//			rc.set(x, 0.0f, w, height);
	//		}

	//		widget->setRect(rc);
	//		x += w + spacing;
	//	}

	//	break;
	//case horus::LayoutType::Grid:
	//{
	//	w = width / widgets.size() - spacing * (widgets.size() - 1);
	//	h = height / widgets.size() - spacing * (widgets.size() - 1);
	//	x = 0.0f;
	//	y = 0.0f;
	//	u32 nextCellIndex = 0;
	//	u32 cellCount = rows * columns;

	//	for (auto widget : widgets)
	//	{
	//		f32 cellWidth = w;
	//		f32 cellHeight = h;
	//		f32 spanRows = 1;
	//		f32 spanCols = 1;

	//		//TODO: this is hard, need paper and pen :D

	//		auto iterCell = gridCellItems.find(widget);

	//		if (iterCell != gridCellItems.end())
	//		{
	//			spanRows = iterCell->second.rowSpan;
	//			spanCols = iterCell->second.colSpan;
	//			rc.set(
	//				iterCell->second.column * (cellWidth + columnSpacing),
	//				iterCell->second.row * (cellHeight + rowSpacing),
	//				cellWidth * spanCols,
	//				cellHeight * spanRows);
	//		}
	//		else
	//		{
	//			u32 nextFreeCellRow = 0;
	//			u32 nextFreeCellCol = 0;

	//			while (nextCellIndex < cellCount)
	//			{
	//				//TODO
	//			}
	//			
	//			rc.set(x, y, w, h);
	//		}

	//		widget->setRect(rc);
	//		y += h + spacing;
	//	}
	//	break;
	//}
	//case horus::LayoutType::Fill:
	//	for (auto widget : widgets)
	//	{
	//		widget->setRect({ 0, 0, width, height });
	//	}

	//	break;
	//default:
	//	break;
	//}
}

}