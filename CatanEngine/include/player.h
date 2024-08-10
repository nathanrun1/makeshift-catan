#pragma once
#include <unordered_map>
#include <string>
#include "resource.h"

//class DevCard;
class Occupation;

class Player {
public:
	std::string name;
	int victory_points = 0;
	int army_size = 0;
	int longest_road = 0;
	//std::vector<DevCard> dev_cards;
	std::vector<Occupation*> occupations;
	std::unordered_map<Resource, int> discounts;
	std::unordered_map<Resource, int> resources;

	Player();
	Player(std::string name);
};