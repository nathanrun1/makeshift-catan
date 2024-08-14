#pragma once

#include "player.h"

class Node;

class Occupation {
public:
	virtual ~Occupation() = default;

	Node* const node;
	Player* const player;

	Occupation(Player* player, Node* node) : player(player), node(node) {}
};