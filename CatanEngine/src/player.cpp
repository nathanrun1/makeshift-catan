#include "player.h"
#include "catan_io.h"

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


std::unique_ptr<DecisionResult> Player::Decide(GameState& game_state, Decision decision) {
	// modify game_state to only contain info available to this player
	// e.g.
	GameState plr_game_state = game_state.data << 1;
	Catan_IO::Info(name + " DECISION");
	
	return agent.Decide(plr_game_state, decision);
}

int Player::GetResourceAmnt() {
	int total = 0;
	for (std::pair<const Resource, int>& inv_pair : resources) {
		total += inv_pair.second;
	}
	return total;
}