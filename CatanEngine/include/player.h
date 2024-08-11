#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "resource.h"

//class DevCard;
class Occupation;

class Player {
public:
	int id; // <- must be unique
	std::string name;
	int victory_points = 0;
	int army_size = 0;
	int longest_road = 0;
	//std::vector<DevCard> dev_cards;
	std::vector<std::shared_ptr<Occupation>> occupations;
	std::unordered_map<Resource, int> discounts;
	std::unordered_map<Resource, int> resources;

	bool operator==(const Player& other) const {
		return this->id == other.id;
	}

	bool operator!=(const Player& other) const {
		return this->id != other.id;
	}

	Player(int id);
	Player(int id, std::string name);
};