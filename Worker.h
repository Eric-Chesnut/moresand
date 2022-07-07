#pragma once

#include "World.h"


class Worker {
protected:
	World& world;
	Chunk* chunk;

public:
	Worker(World& worldd, Chunk* chunkk);

	void UpdateChunk();

	Cell& GetCell(int x, int y);

	void SetCell(int x, int y, const Cell& cell);

	void MoveCell(int x, int y, int xto, int yto);

	bool InBounds(int x, int y);

	bool IsEmpty(int x, int y);

	virtual void UpdateCell(int x, int y, Cell& cell) = 0;
};