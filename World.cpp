#include "World.h"



World::World(size_t chunkWidth, size_t chunkHeight, int worldW, int worldH)
	: chunkWidth(chunkWidth)
	, chunkHeight(chunkHeight)
	, worldW(worldW)
	, worldH(worldH)
{
	//InitTheWorld();
}

World::~World()
{
	for (Chunk* chunk : chunks)
	{
		delete chunk;
	}
}



inline std::pair<int, int> World::GetChunkLocation(Chunk* chunk)
{
	return GetChunkLocation(chunk->x, chunk->y);
}

Cell& World::GetCell(int x, int y)
{
	return GetChunk(x, y)->GetCell(x, y);
}

void World::SetCell(int x, int y,const Cell& cell)
{
	if (Chunk* chunk = GetChunk(x, y)) {
		chunk->SetCell(x, y, cell);
	}
}


void World::MoveCell(int x, int y,int xto, int yto)
{
	if (Chunk* src = GetChunk(x, y))
		if (Chunk* dest = GetChunk(xto, yto)) {
			dest->MoveCell(src, x, y, xto, yto);
		}
}

bool World::InBounds(int x, int y)
{
	if (Chunk* chunk = GetChunk(x, y)) {
		return chunk->InBounds(x, y);
	}

	return false;
}

bool World::IsEmpty(int x, int y)
{
	return InBounds(x, y) && GetChunk(x, y)->IsEmpty(x, y);
}


void World::KeepAlive(int x, int y)
{
	if (Chunk* chunk = GetChunk(x, y)) {
		chunk->KeepAlive(x, y);
	}
}


std::pair<int, int> World::GetChunkLocation(int x, int y)
{
	return {floor(float(x) / chunkWidth),floor(float(y) / chunkHeight) };
}


Chunk* World::GetChunk(int x, int y)
{
	return GetChunkL(GetChunkLocation(x, y));
}


Chunk* World::GetChunkL(std::pair<int, int> location)
{
	Chunk* chunk = GetChunkDirect(location);
	return chunk ? chunk : CreateChunk(location);

}

Chunk* World::GetChunkDirect(std::pair<int, int> location)
{
	auto itr = chunkLookup.find(location);
	if (itr == chunkLookup.end()) {
		return nullptr;
	}

	return itr->second;
}


Chunk* World::CreateChunk(std::pair<int, int> location)
{
	

	auto [lx, ly] = location;

	if (lx < -0 || ly < 0
		|| lx >  worldW || ly >  worldH) // could pass in a world limit to constructor
	{
		return nullptr;
	}

	Chunk* chunk = new Chunk(chunkWidth, chunkHeight, lx, ly);


	chunkLookup.insert({ location, chunk });

	std::unique_lock lock(chunkMutex); // lock chunks

	chunks.push_back(chunk);

	return chunk;
}


void World::RemoveEmptyChunks() {

	

	for (size_t i = 0; i < chunks.size(); i++) {
		Chunk* chunk = chunks.at(i);

		if (chunk->filledCellCount == 0 && chunk->deleteMe == false) //delay by one frame, may have been my problem before
		{
			chunk->deleteMe = true;
			continue;
		}
		/*
		if (chunk->filledCellCount != 0)
		{
			chunk->deleteMe = false;
			continue;
		}*/

		if (chunk->deleteMe) 
		{
			
			chunkLookup.unsafe_erase(GetChunkLocation(chunk->x, chunk->y));
			chunks[i] = chunks.back(); chunks.pop_back();
			i--;

			delete chunk;
		}
	}
}



void World::InitTheWorld()
{
	for (int x = 0; x <= worldW; x++)
	{
		for (int y = 0; y <= worldH; y++)
		{
			CreateChunk({ x, y });
		}
	}
}


void World::UpdateRect()
{
	for (int i = 0; i < size(chunks); i++) {
		Chunk* chunk = chunks[i];
		chunk->UpdateRect();
	}
}
