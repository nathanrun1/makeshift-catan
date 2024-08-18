#pragma once

#include <bitset>

class GameState {
public:
	std::bitset<32> data;

	GameState(std::bitset<32> data) : data(data) {}
};