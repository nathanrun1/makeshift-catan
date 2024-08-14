#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "resource.h"

//class DevCard;
class Occupation;

class Player {
private:
	static int player_count;
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

	/// <summary>
	/// Compares equality of two players. Equality is determined by Player.id
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	bool operator==(const Player& other) const;

	/// <summary>
	/// Compares inequality of two players. Equality is determined by Player.id
	/// </summary>
	/// <param name="other"></param>
	/// <returns></returns>
	bool operator!=(const Player& other) const;

	Player();
	Player(std::string name);
};