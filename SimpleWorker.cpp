#include "SimpleWorker.h"


SimpleWorker::SimpleWorker(World& world, Chunk* chunk) : Worker(world, chunk) 
{}

void SimpleWorker::UpdateCell(int x, int y, Cell& cell)
{
	if (cell.Props & CellProperties::MOVE_DOWN && MoveDown(x, y, cell)) {}
	else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell)) {}
	else if (cell.Props & CellProperties::MOVE_SIDE && MoveSide(x, y, cell)) {}
}

bool SimpleWorker::MoveDown(int x, int y, const Cell& cell) 
{
    bool down = world.IsEmpty(x, y + 1);
    if (down) {
        world.MoveCell(x, y, x, y + 1);
    }

    return down;
}


bool SimpleWorker::MoveDownSide(int x, int y, const Cell& cell) 
{
    bool downLeft = world.IsEmpty(x - 1, y + 1);
    bool downRight = world.IsEmpty(x + 1, y + 1);

    if (downLeft && downRight) {
        downLeft = (rand() % 2) > 0;
        downRight = !downLeft;
    }

    if (downLeft)  world.MoveCell(x, y, x - 1, y + 1);
    else if (downRight) world.MoveCell(x, y, x + 1, y + 1);

    return downLeft || downRight;
}


bool SimpleWorker::MoveSide(int x, int y, const Cell& cell)
{
    bool left = world.IsEmpty(x - 1, y);
    bool right = world.IsEmpty(x + 1, y);

    if (left && right) {
        left = (rand() % 2) > 0;
        right = !left;
    }

    if (left)  world.MoveCell(x, y, x - 1, y);
    else if (right) world.MoveCell(x, y, x + 1, y);

    return left || right;
}