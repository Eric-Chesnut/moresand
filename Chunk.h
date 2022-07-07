#pragma once

#include "Cell.h"
#include <functional>
#include <algorithm>
#include <vector>
#include <mutex>
#include <stdlib.h>     /* srand, rand */


class Chunk;


class Chunk {
public:
	const int width, height;
	const int x, y;
	int minX, minY, maxX, maxY; //dirty rect
	int filledCellCount;
	bool deleteMe;
	Cell* cells;
	Cell _EMPTY;
private:
	std::vector<std::tuple<Chunk*, int, int>> changes; // destination, source
	int minXt, minYt, maxXt, maxYt; //working dirty rect
	void InitializeCells();
public:
	Chunk(int widtht, int heightt, int xt, int yt);
	~Chunk();
	int GetIndex(int xs, int ys); 
	bool InBounds(int xs, int ys);
	bool IsEmpty(int xs, int ys);
	bool IsEmpty(int index);
	Cell& GetCell(int xs, int ys);
	Cell& GetCell(int index);
	void SetCell(int xs, int ys, const Cell& cell);
	void SetCell(int index, const Cell& cell);
	void MoveCell(Chunk* source, int xs, int ys, int xto, int yto);
	void CommitCells();
	void KeepAlive(int xs, int ys);
	void KeepAlive(int index);
	void UpdateRect();
};