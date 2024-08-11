#pragma once

#include "player.h"

class Node;

class Occupation {
public:
	virtual ~Occupation() = default;

	const Node* node;
	const Player* player;

	Occupation(Player* player, Node* node) : player(player), node(node) {}
};