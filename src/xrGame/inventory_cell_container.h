#pragma once
class FakeCellContainer
{
public:
	Ivector2		m_cellsCapacity;
	xr_vector<byte> m_cells;
public:
					FakeCellContainer();
	virtual			~FakeCellContainer();

	void			Init(int col, int row);
	void			UpdateSize(int col, int row);

	byte&			GetCellAt(const Ivector2& pos);
	bool			ValidCell(const Ivector2& pos);

	Ivector2		FindFreeCell(const Ivector2& _size);
	bool			HasFreeSpace(const Ivector2& size);
	bool			IsRoomFree(const Ivector2& pos, const Ivector2& size);

	bool			CanSetItem(const Ivector2& size, bool bVert = false);
	void			SetItem(const Ivector2& pos, const Ivector2& size, bool bVert = false);
	void			SetItem(const Ivector2& size, bool bVert = false);
	void			RemoveItem(const Ivector2& pos, const Ivector2& _size);
};

