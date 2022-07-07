
#include "Chunk.h"



Chunk::Chunk(int widtht, int heightt, int xt, int yt)
	: width(widtht)
	, height(heightt)
	, x(xt* widtht)
	, y(yt* heightt)
	, filledCellCount(0)
	, deleteMe(false)
{
	cells = new Cell[width * height];
	Cell setCell = _EMPTY;

	for (int i = 0; i < width * height; i++) //fills the new chunk with empty
	{

		cells[i] = setCell;
	}
	UpdateRect();
	UpdateRect();
}

Chunk::~Chunk()
{
	delete[] cells;
}

void Chunk::InitializeCells()
{
	_EMPTY = {
		CellType::EMPTY,
		CellProperties::NONE,
		0, 0, 0, 0 // 0 alpha allows for a background
	};
}


Cell& Chunk::GetCell(int xs, int ys)
{
	return GetCell(GetIndex(xs, ys));
}


Cell& Chunk::GetCell(int index)
{
	return cells[index];
}


void Chunk::SetCell(int xs, int ys, const Cell& cell)
{
	SetCell(GetIndex(xs, ys), cell);
}

//***INCOMPLETE*** come back to function down the line
void Chunk::SetCell(int index, const Cell& cell)
{
	

	Cell& dest = cells[index];
	if (dest.Type == CellType::EMPTY && cell.Type != CellType::EMPTY) // Filling a cell
	{
		filledCellCount++;
	}

	else if (dest.Type != CellType::EMPTY && cell.Type == CellType::EMPTY) // Removing a filled cell
		{
			filledCellCount--;
		}
	dest = cell;

	KeepAlive(index);
}


void Chunk::MoveCell(Chunk* source, int xs, int ys, int xto, int yto)
{
	changes.emplace_back(source, source->GetIndex(xs, ys), GetIndex(xto, yto));
}

void Chunk::CommitCells() 
{
#define _DEST 2

	// remove moves that have their destinations filled

	for (size_t i = 0; i < changes.size(); i++) {
		const Cell& dest = GetCell(std::get<_DEST>(changes[i]));

		if (dest.Type != CellType::EMPTY) {
			changes[i] = changes.back(); changes.pop_back();
			i--;
		}
	}

	// sort by destination

	std::sort(changes.begin(), changes.end(),
		[](auto& a, auto& b) { return std::get<_DEST>(a) < std::get<_DEST>(b); }
	);

	// pick random source for each destination

	size_t iprev = 0;

	changes.emplace_back(nullptr, -1, -1); // to catch final move

	for (size_t i = 0; i < changes.size() - 1; i++) {
		if (std::get<_DEST>(changes[i + 1]) != std::get<_DEST>(changes[i])) {
			size_t randy = iprev + (rand()%(i - iprev + 1));

			auto [chunk, src, dst] = changes[randy];

			SetCell(dst, chunk->GetCell(src));
			chunk->SetCell(src, Cell());

			iprev = i + 1;
		}
	}

	changes.clear();
#undef _DEST

	//***INCOMPLETE*** needed for update rect

}


int Chunk::GetIndex(int xs, int ys)
{
	xs -= x;
	ys -= y;
	return xs + ys * width;
}

bool Chunk::InBounds(int xs, int ys)
{
	return xs >= x && xs < x + width
		&& ys >= y && ys < y + height;
}

bool Chunk::IsEmpty(int xs, int ys)
{
	return IsEmpty(GetIndex(xs,ys));
}

bool Chunk::IsEmpty(int index)
{
	return GetCell(index).Type == CellType::EMPTY;
}


void Chunk::KeepAlive(int xs, int ys)
{
	KeepAlive(GetIndex(xs, ys));
}

void Chunk::KeepAlive(int index)
{
	int xt = index % width;
	int yt = index / width;

	minXt = std::clamp(std::min(xt - 2, minXt), 0, width);
	minYt = std::clamp(std::min(yt - 2, minYt), 0, height);
	maxXt = std::clamp(std::max(xt + 2, maxXt), 0, width);
	maxYt = std::clamp(std::max(yt + 2, maxYt), 0, height);
}

void Chunk::UpdateRect() 
{
	// Update current; reset working
	minX = minXt;  minXt = width;
	minY = minYt;  minYt = height;
	maxX = maxXt;  maxXt = -1;
	maxY = maxYt;  maxYt = -1;
}