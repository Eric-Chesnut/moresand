#pragma once
#include "Worker.h"

class SimpleWorker : public Worker {
public:
	SimpleWorker(World& world, Chunk* chunk); 

	void UpdateCell(int x, int y, Cell& cell) override;
private:
	bool MoveDown(int x, int y, const Cell& cell); 
	bool MoveDownSide(int x, int y, const Cell& cell);
	bool MoveSide(int x, int y, const Cell& cell); 
};