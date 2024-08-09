#include "player.h"

Player::Player() : name("Player") {
	for (const Resource& rsc : RESOURCES) {
		discounts[rsc] = 4;
		resources[rsc] = 0;
	}
}

Player::Player(std::string name) : Player() {
	this->name = name;
}

