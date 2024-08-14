#include "player.h"

int Player::player_count = 0;

Player::Player() : id(player_count), name("Player") {
	player_count++;
	for (const Resource& rsc : RESOURCES) {
		discounts[rsc] = 4;
		resources[rsc] = 0;
	}
}

Player::Player(std::string name) : Player() {
	this->name = name;
}

bool Player::operator==(const Player& other) const {
	return this->id == other.id;
}

bool Player::operator!=(const Player& other) const {
	return this->id != other.id;
}
