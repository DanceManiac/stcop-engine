#include "stdafx.h"
#include "inventory_cell_container.h"

FakeCellContainer::FakeCellContainer()
{
	m_cellsCapacity.set(0,0);
}

FakeCellContainer::~FakeCellContainer()
{
	
}

void FakeCellContainer::Init(int col, int row)
{
	m_cells.resize(col * row,0);
	m_cellsCapacity.set(col,row);
}

void FakeCellContainer::UpdateSize(int col, int row)
{
	m_cells.clear();
	m_cells.resize(col * row,0);
	m_cellsCapacity.set(col, row);
}

byte& FakeCellContainer::GetCellAt(const Ivector2& pos)
{
	R_ASSERT(ValidCell(pos));
	byte& c = m_cells[m_cellsCapacity.x * pos.y + pos.x];
	return				c;
}
bool FakeCellContainer::ValidCell(const Ivector2& pos)
{
	return !(pos.x < 0 || pos.y < 0 || pos.x >= m_cellsCapacity.x || pos.y >= m_cellsCapacity.y);
}

Ivector2 FakeCellContainer::FindFreeCell(const Ivector2& _size)
{
	Ivector2 tmp;
	Ivector2 size = _size;

	for (tmp.y = 0; tmp.y <= m_cellsCapacity.y - size.y; ++tmp.y)
		for (tmp.x = 0; tmp.x <= m_cellsCapacity.x - size.x; ++tmp.x)
			if (IsRoomFree(tmp, _size))
				return  tmp;

	return Ivector2().set(-1,-1);
}
bool FakeCellContainer::HasFreeSpace(const Ivector2& _size)
{
	Ivector2 tmp;
	Ivector2 size = _size;

	for (tmp.y = 0; tmp.y <= m_cellsCapacity.y - size.y; ++tmp.y)
		for (tmp.x = 0; tmp.x <= m_cellsCapacity.x - size.x; ++tmp.x)
			if (IsRoomFree(tmp, _size))
				return true;

	return false;
}
bool FakeCellContainer::IsRoomFree(const Ivector2& pos, const Ivector2& _size)
{
	Ivector2 tmp;
	Ivector2 size = _size;

	for (tmp.x = pos.x; tmp.x < pos.x + size.x; ++tmp.x)
		for (tmp.y = pos.y; tmp.y < pos.y + size.y; ++tmp.y)
		{
			if (!ValidCell(tmp))		return		false;

			byte& C = GetCellAt(tmp);

			if (!C == 0)			return		false;
		}
	return true;
}

void    FakeCellContainer::RemoveItem(const Ivector2& pos, const Ivector2& _size)
{
	for (int x = 0; x < _size.x; ++x)
		for (int y = 0; y < _size.y; ++y)
		{
			byte& C = GetCellAt(Ivector2().set(x, y).add(pos));
			C = 0;
		}
}

bool	FakeCellContainer::CanSetItem(const Ivector2& size, bool bVert)
{
	Ivector2 _size = size;

	if (bVert)	std::swap(_size.x, _size.y);

	return HasFreeSpace(_size);
}
void	FakeCellContainer::SetItem(const Ivector2& pos, const Ivector2& size, bool bVert)
{
	Ivector2 _size = size;

	if (bVert)	std::swap(_size.x, _size.y);

	if (IsRoomFree(pos, _size))
	{
		for (int x = 0; x < _size.x; ++x)
			for (int y = 0; y < _size.y; ++y)
			{
				byte& C = GetCellAt(Ivector2().set(x, y).add(pos));
				C = 1;
			}
	}
	else
		SetItem(_size, bVert);
}
void	FakeCellContainer::SetItem(const Ivector2& size, bool bVert)
{
	Ivector2 _size = size;

	if (bVert)	std::swap(_size.x, _size.y);

	Ivector2 pos = FindFreeCell(_size);

	if (pos.x != -1 && pos.y != -1)
	{
		for (int x = 0; x < _size.x; ++x)
			for (int y = 0; y < _size.y; ++y)
			{
				byte& C = GetCellAt(Ivector2().set(x, y).add(pos));
				C = 1;
			}
	}
}