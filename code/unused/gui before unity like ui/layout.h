#pragma once
#include "core/defines.h"
#include "core/types.h"
#include "base/math/rect.h"
#include "base/array.h"
#include "base/map.h"

namespace engine
{
class Widget;

enum class LayoutSizingPolicy
{
	Keep,
	Resize
};

struct GridCellSpan
{
	GridCellSpan(u32 Rows, u32 Columns)
		: rows(Rows)
		, columns(Columns)
	{}

	u32 rows = 1;
	u32 columns = 1;
};

struct GridCellCoords
{
	GridCellCoords(u32 cellRow, u32 cellColumn)
		: row(cellRow)
		, column(cellColumn)
	{}

	u32 row = 0;
	u32 column = 0;
};

struct LayoutGridSizeInfo
{
	bool fixed = false;
	u32 stretch = 1;
	f32 minSize = 0.0f;
};

struct LayoutGridCellItem
{
	u32 row = 0;
	u32 column = 0;
	u32 rowSpan = 1;
	u32 colSpan = 1;
};

class ENGINE_API Layout
{
public:
	Layout() {}
	void setSize(f32 w, f32 h) { width = w; height = h; }
	void arrangeWidgets(const Array<Widget*>& widgets);

protected:
	Layout* parent = nullptr;
	Widget* parentWidget = nullptr;
	f32 width = 0.0f;
	f32 height = 0.0f;
	f32 spacing = 2.0f;
	Rect padding;
	LayoutSizingPolicy sizingPolicy = LayoutSizingPolicy::Resize;
	f32 rowSpacing = 2.0f;
	f32 columnSpacing = 2.0f;
	u32 rows = 1;
	u32 columns = 1;
	Array<LayoutGridSizeInfo> rowSizes;
	Array<LayoutGridSizeInfo> columnSizes;
	Map<Widget*, LayoutGridCellItem> gridCellItems;
};


}