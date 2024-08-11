#pragma once

#include "mapcomponents.h"

class Settlement : public Occupation {
public:
	const int victory_points = 1;
	const int production = 1;

	Settlement(Player* player, Node* node) : Occupation(player, node) {}
};

class City : public Occupation {
public:
	const int victory_points = 2;
	const int production = 2;

	City(Player* player, Node* node) : Occupation(player, node) {}
};