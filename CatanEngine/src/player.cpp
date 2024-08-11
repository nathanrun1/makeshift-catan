#include "player.h"

Player::Player(int id) : id(id), name("Player") {
	for (const Resource& rsc : RESOURCES) {
		discounts[rsc] = 4;
		resources[rsc] = 0;
	}
}

Player::Player(int id, std::string name) : Player(id) {
	this->name = name;
}

