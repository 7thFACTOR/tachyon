#pragma once
#include "base/array.h"
#include "base/defines.h"
#include "base/types.h"
#include "base/assert.h"

namespace engine
{
template<typename _Type> class QuadTree;
template<typename _Type> class QuadTreeCell;

template<typename _Type>
class E_API QuadTreeCell
{
public:
	QuadTreeCell(
		QuadTreeCell* pParent = nullptr,
		f32 x = 0, f32 y = 0,
		f32 width = 0, f32 height = 0);
	~QuadTreeCell();

	void createChildren();
	void free();
	bool isLeaf() const;
	bool isPointInside(f32 x, f32 y) const;

protected:
	QuadTreeCell<_Type>* m_pParent;
	QuadTreeCell<_Type>* m_children[4];
	Array<_Type> m_elements;
	f32 m_x, m_y, m_width, m_height;
};

template<typename _Type>
class E_API QuadTree
{
public:
	QuadTree();
	virtual ~QuadTree();

	void create(f32 x, f32 y, f32 width, f32 height, u32 maxDepth = -1);
	bool hitTest(Array<_Type>& outHitElements, f32 x, f32 y, f32 width = 0, f32 height = 0);
	bool hitTestRadius(Array<_Type>& outHitElements, f32 x, f32 y, f32 radius);
	void free();
	virtual bool subdivide(QuadTreeCell<_Type>* pCrtCell) = 0;
	QuadTreeCell<_Type>& rootCell();

protected:
	void clearElementsOfNonLeafCells();
	void createCellRecursive(QuadTreeCell<_Type>* pParent, u32 crtDepth, u32 maxDepth);
	void clearElementsOfNonLeafCellsRecursive(QuadTreeCell<_Type>* pCell);
	void hitTestRecursive(
		Array<_Type>& outHitElements, 
		QuadTreeCell<_Type>* pCell,
		f32 x, f32 y,
		f32 width = 0, f32 height = 0);
	void hitTestRadiusRecursive(
		Array<_Type>& outHitElements,
		QuadTreeCell<_Type>* pCell,
		f32 x, f32 y,
		f32 radius);

	QuadTreeCell<_Type> m_rootCell;
	u32 m_cellCount;
};

//---

template<typename _Type>
QuadTreeCell<_Type>::QuadTreeCell(
	QuadTreeCell<_Type>* pParent,
	f32 x, f32 y,
	f32 width, f32 height)
{
	m_pParent = pParent;
	m_children[0] = m_children[1] = m_children[2] = m_children[3] = nullptr;
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
}

template<typename _Type>
QuadTreeCell<_Type>::~QuadTreeCell()
{
	free();
}

template<typename _Type>
void QuadTreeCell<_Type>::createChildren()
{
	f32 halfW = m_width / 2.0f;
	f32 halfH = m_height / 2.0f;

	m_children[0] = new QuadTreeCell<_Type>(this, m_x, m_y, halfW, halfH);
	m_children[1] = new QuadTreeCell<_Type>(this, m_x + halfW, m_y, halfW, halfH);
	m_children[2] = new QuadTreeCell<_Type>(this, m_x + halfW, m_y + halfH, halfW, halfH);
	m_children[3] = new QuadTreeCell<_Type>(this, m_x, m_y + halfH, halfW, halfH);
	B_ASSERT(m_children[0]);
	B_ASSERT(m_children[1]);
	B_ASSERT(m_children[2]);
	B_ASSERT(m_children[3]);
}

template<typename _Type>
void QuadTreeCell<_Type>::free()
{
	B_SAFE_DELETE(m_children[0]);
	B_SAFE_DELETE(m_children[1]);
	B_SAFE_DELETE(m_children[2]);
	B_SAFE_DELETE(m_children[3]);
}

template<typename _Type>
bool QuadTreeCell<_Type>::isLeaf() const
{
	return nullptr == m_children[0];
}

template<typename _Type>
bool QuadTreeCell<_Type>::isPointInside(f32 x, f32 y) const
{
	return m_x < x
		&& (m_x + m_width) > x
		&& m_y < y
		&& (m_y + m_height) > y;
}

//---

template<typename _Type>
QuadTree<_Type>::QuadTree()
{}

template<typename _Type>
QuadTree<_Type>::~QuadTree()
{
	free();
}

template<typename _Type>
void QuadTree<_Type>::create(f32 x, f32 y, f32 width, f32 height, uint32 maxDepth)
{
	free();
	m_cellCount = 0;
	m_rootCell.m_x = x;
	m_rootCell.m_y = y;
	m_rootCell.m_width = width;
	m_rootCell.m_height = height;
	createCellRecursive(&m_rootCell, 0, maxDepth);
	clearElementsOfNonLeafCells();
}

template<typename _Type>
void QuadTree<_Type>::clearElementsOfNonLeafCells()
{
	clearElementsOfNonLeafCellsRecursive(&m_rootCell);
}

template<typename _Type>
bool QuadTree<_Type>::hitTest(Array<_Type>& outElements, f32 x, f32 y, f32 width, f32 height)
{
	outElements.clear();
	hitTestRecursive(outElements, &m_rootCell, x, y, width, height);

	return 0 != outElements.size();
}

template<typename _Type>
void QuadTree<_Type>::hitTestRecursive(
	Array<_Type>& outElements,
	QuadTreeCell<_Type>* pCell,
	f32 x, f32 y,
	f32 width, f32 height)
{
	if (!pCell)
		return;

	// if not inside cell, return
	if ((x < pCell->m_x && x + width < pCell->m_x
		|| x > pCell->m_x + pCell->m_width && x + width > pCell->m_x + pCell->m_width)
		&&
		(y < pCell->m_y && y + height < pCell->m_y
		|| y > pCell->m_y + pCell->m_height && y + height > pCell->m_y + pCell->m_height))
		return;

	// if leaf, return the elements
	if (pCell->isLeaf())
	{
		outElements.insert(outElements.end(), pCell->m_elements.begin(), pCell->m_elements.end());
		return;
	}

	hitTestRecursive(outElements, pCell->m_children[0], x, y, width, height);
	hitTestRecursive(outElements, pCell->m_children[1], x, y, width, height);
	hitTestRecursive(outElements, pCell->m_children[2], x, y, width, height);
	hitTestRecursive(outElements, pCell->m_children[3], x, y, width, height);
}

template<typename _Type>
bool QuadTree<_Type>::hitTestRadius(Array<_Type>& outElements, f32 x, f32 y, f32 radius)
{
	outElements.clear();
	hitTestRadiusRecursive(outElements, &m_rootCell, x, y, radius);
	return !outElements.isEmpty();
}

template<typename _Type>
void QuadTree<_Type>::hitTestRadiusRecursive(
	Array<_Type>& outElements,
	QuadTreeCell<_Type>* pCell,
	f32 x, f32 y, f32 radius)
{
	if (!pCell)
		return;

	// Find the closest point to the circle within the rectangle
	f32 closestX = clampValue(x, pCell->m_x, pCell->m_x + pCell->m_width);
	f32 closestY = clampValue(y, pCell->m_y, pCell->m_y + pCell->m_height);

	// Calculate the distance between the circle's center and this closest point
	f32 distanceX = x - closestX;
	f32 distanceY = y - closestY;

	// If the distance is less than the circle's radius, an intersection occurs
	f32 distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

	if (distanceSquared > (radius * radius))
		return;

	// if leaf, return the elements
	if (pCell->isLeaf())
	{
		//TODO:
		outElements.insert(outElements.end(), pCell->m_elements.begin(), pCell->m_elements.end());

		return;
	}

	hitTestRadiusRecursive(outElements, pCell->m_children[0], x, y, radius);
	hitTestRadiusRecursive(outElements, pCell->m_children[1], x, y, radius);
	hitTestRadiusRecursive(outElements, pCell->m_children[2], x, y, radius);
	hitTestRadiusRecursive(outElements, pCell->m_children[3], x, y, radius);
}

template<typename _Type>
void QuadTree<_Type>::free()
{
	m_rootCell.free();
}

template<typename _Type>
void QuadTree<_Type>::createCellRecursive(QuadTreeCell<_Type>* pParent, uint32 crtDepth, uint32 maxDepth)
{
	if (crtDepth >= maxDepth && maxDepth != -1)
		return;

	if (!pParent)
		return;

	++m_cellCount;

	// user didn't allowed further division
	if (!subdivide(pParent))
		return;

	++crtDepth;

	if (pParent->m_children[0])
	{
		createCellRecursive(pParent->m_children[0], crtDepth, maxDepth);
	}

	if (pParent->m_children[1])
	{
		createCellRecursive(pParent->m_children[1], crtDepth, maxDepth);
	}

	if (pParent->m_children[2])
	{
		createCellRecursive(pParent->m_children[2], crtDepth, maxDepth);
	}

	if (pParent->m_children[3])
	{
		createCellRecursive(pParent->m_children[3], crtDepth, maxDepth);
	}
}

template<typename _Type>
void QuadTree<_Type>::clearElementsOfNonLeafCellsRecursive(QuadTreeCell<_Type>* pCell)
{
	if (pCell->isLeaf())
		return;

	pCell->m_elements.clear();
	clearElementsOfNonLeafCellsRecursive(pCell->m_children[0]);
	clearElementsOfNonLeafCellsRecursive(pCell->m_children[1]);
	clearElementsOfNonLeafCellsRecursive(pCell->m_children[2]);
	clearElementsOfNonLeafCellsRecursive(pCell->m_children[3]);
}

template<typename _Type>
QuadTreeCell<_Type>& QuadTree<_Type>::rootCell()
{
	return m_rootCell;
}

}