#include "Worker.h"


Worker::Worker(World& worldd, Chunk* chunkk)
	: world(worldd)
	, chunk(chunkk)
{}



void Worker::UpdateChunk() {
	for (int x = chunk->minX; x < chunk->maxX; x++)
		for (int y = chunk->minY; y < chunk->maxY; y++) 
		{
			Cell& cell = chunk->GetCell(x + y * chunk->width);

			int px = x + chunk->x;
			int py = y + chunk->y;

			UpdateCell(px, py, cell);
		}
}


Cell& Worker::GetCell(int x, int y) 
{
	if (chunk->InBounds(x, y))
	{
		return chunk->GetCell(x, y);
	}

	return world.GetCell(x, y);
}

void Worker::SetCell(int x, int y, const Cell& cell) 
{
	if (chunk->InBounds(x, y)) 
	{
		return chunk->SetCell(x, y, cell);
	}

	return world.SetCell(x, y, cell);
}

void Worker::MoveCell(int x, int y, int xto, int yto) 
{
	int pingX = 0, pingY = 0;

	if (x == chunk->x)                         pingX = -1;
	if (x == chunk->x + chunk->width - 1) pingX = 1;
	if (y == chunk->y)                         pingY = -1;
	if (y == chunk->y + chunk->height - 1) pingY = 1;

	if (pingX != 0)               world.KeepAlive(x + pingX, y);
	if (pingY != 0)               world.KeepAlive(x, y + pingY);
	if (pingX != 0 && pingY != 0) world.KeepAlive(x + pingX, y + pingY);

	if (chunk->InBounds(x, y) && chunk->InBounds(xto, yto))
	{
		return chunk->MoveCell(chunk, x, y, xto, yto);
	}

	return world.MoveCell(x, y, xto, yto);
}

bool Worker::InBounds(int x, int y)
{
	return chunk->InBounds(x, y) || world.InBounds(x, y);
}

bool Worker::IsEmpty(int x, int y) 
{
	if (chunk->InBounds(x, y))
	{
		return chunk->IsEmpty(x, y);
	}

	return world.IsEmpty(x, y);
}