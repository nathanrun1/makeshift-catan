#pragma once

#include "occupation.h"
#include "player.h"
#include "mapcomponents.h"

class Settlement : Occupation {
public:
	int victory_points = 1;
	int production = 1;
	Player& player;
	Settlement(Player& player, Node& node) : player(player), Occupation(node) {}
};

class City : Settlement {
public:
	int victory_points = 2;
	int production = 2;

	City(Player& player, Node& node) : Settlement(player, node) {}
};