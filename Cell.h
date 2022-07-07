#pragma once
#include <cstdint>

enum class CellProperties {
	NONE = 0b00000000,
	MOVE_DOWN = 0b00000001,
	MOVE_DOWN_SIDE = 0b00000010,
	MOVE_SIDE = 0b00000100
};

enum class CellType {
	EMPTY,
	SAND,
	WATER,
	ROCK
};

struct Cell {
	CellType       Type = CellType::EMPTY;
	CellProperties Props = CellProperties::NONE;
	int red = 0;
	int green = 0;
	int blue = 0;
	int alpha = 255; // rgba
};

inline CellProperties operator|(
	CellProperties a, CellProperties b)
{
	return CellProperties(static_cast<int>(a) | static_cast<int>(b));
}

inline auto operator&(
	CellProperties a, CellProperties b)
{
	return static_cast<int>(a) & static_cast<int>(b);
}