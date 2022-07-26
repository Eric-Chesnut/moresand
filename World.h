#pragma once
#pragma once
#include "Cell.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include "Chunk.h"
#include <unordered_map>
#include "Hash.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <concurrent_unordered_map.h>



class World {
public:
	const size_t chunkWidth;
	const size_t chunkHeight;
	std::vector<Chunk*> chunks;
	const int worldW; // limit of the world in x direction
	const int worldH; // limit of the world in y direction

private:
	// concurrent unordered map so it can safely be accessed by multiple threads
	// is microsoft class
	Concurrency::concurrent_unordered_map<std::pair<int, int>, Chunk*, pair_hash> chunkLookup;
	Chunk* CreateChunk(std::pair<int, int> location);
	std::mutex chunkMutex; //chunk access mutex

	

public:
	World(size_t chunkWidth, size_t chunkHeight, int worldW, int worldH);

	~World();

	bool InBounds(int x, int y);

	bool IsEmpty(int x, int y);

	Cell& GetCell(int x, int y);

	void SetCell(int x, int y, const Cell& cell);

	void MoveCell(int x, int y, int xto, int yto);

	Chunk* GetChunk(int x, int y);

	Chunk* GetChunkDirect(std::pair<int, int> location);

	inline std::pair<int, int> GetChunkLocation(int x, int y);

	inline std::pair<int, int>GetChunkLocation(Chunk* chunk);

	void KeepAlive(int x, int y);

	void RemoveEmptyChunks();

	Chunk* GetChunkL(std::pair<int, int> location);

	void InitTheWorld();

	void UpdateRect();
};