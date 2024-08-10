#pragma once

#include "resource.h"
#include "occupation.h"

class Player;

class Hex {
public:
	int dice_num;
	Resource resource;
	std::pair<int, int> pos;

	Hex(std::pair<int, int> pos, int dice_num, Resource resource)
		: pos(pos), dice_num(dice_num), resource(resource) {}
};

class Node {
public:
	std::optional<Occupation> occ;
	std::vector<Hex*> adj_hexes;
	std::vector<std::pair<Node*, Player*>> roads;
	std::pair<int, int> pos;

	Node(std::pair<int, int> pos) : pos(pos), occ(std::nullopt) {}
};