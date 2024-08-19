#define _CATAN_DEBUG_
#include "game.h"


Game::Game(int vp_to_win) : largest_army_plr(nullptr), longest_road_plr(nullptr), 
vp_to_win(vp_to_win), m_gen(m_rd()), game_state(std::bitset<32>()) {}

int Game::DiceRoll() {
	std::uniform_int_distribution dist(1, 6);
	return dist(m_gen) + dist(m_gen);
}


void Game::GetPlayers() {
	for (int i = 0; i < 4; ++i) {
		players.push_back(Player("Player" + std::to_string(i + 1)));
	}
}

void Game::SetupTurn(Player& player) {
	// SETTLEMENT PLACEMENT:
	std::unique_ptr<PositionDecisionResult>
		settle_decision(dynamic_cast<PositionDecisionResult*>(player.Decide(game_state, Decision::POS_IniSettle).release()));
	std::pair<int, int> settle_pos(0, 0);
	if (!(settle_decision && settle_decision->positions.size() > 0 && BuildSettlement(player, settle_pos = settle_decision->positions[0], true))) {
		// Assumes valid position exists, in case that decision is invalid 
		BuildSettlement(player, settle_pos = map.OccGetRandPos(player, false).value(), true);
	}
	// ROAD PLACEMENT:
	std::unique_ptr<PositionDecisionResult>
		road_decision(dynamic_cast<PositionDecisionResult*>(player.Decide(game_state, Decision::POS_IniRoad).release()));
	if (!(road_decision && road_decision->positions.size() >= 2 &&
		(road_decision->positions[0] == settle_pos || road_decision->positions[1] == settle_pos) &&
		BuildRoad(player, road_decision->positions[0], road_decision->positions[1]))) {
		// Decision invalid, place road and random valid spot
		std::pair<std::pair<int, int>, std::pair<int, int>> rand_spot = map.RoadGetRandPos(player,{map.node_grid[settle_pos.first][settle_pos.second].value().occ}).value();
		BuildRoad(player, rand_spot.first, rand_spot.second);
	}
}

void Game::SetupPhase() {
	for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it) {
		SetupTurn(*it);
	}
	for (std::vector<Player>::iterator it = players.end(); it != players.begin(); --it) {
		SetupTurn(*(it - 1));
	}
}

void Game::PlayerTurn(Player& player) {
	Catan_IO::Info(player.name + "'s TURN");
	int dice_roll = DiceRoll();
	std::vector<Bank::Request> requests;
	if (dice_roll != 7) {
		for (Player& player : players) {
			for (std::shared_ptr<Occupation>& occ_ptr : player.occupations) {
				std::shared_ptr<Settlement> settlement_ptr = std::dynamic_pointer_cast<Settlement>(occ_ptr);
				if (settlement_ptr) {
					std::vector<std::pair<Resource, int>> resources = map.GetResources(*settlement_ptr);
					for (std::pair<Resource, int>& rsc : resources) {
						if (rsc.second == dice_roll) {
							requests.push_back(Bank::Request(&player, rsc.first, settlement_ptr->production));
						}
					}
				}
			}
			bank.ProcessMultiple(requests);
		}
	}
	else {
		// Prompt player to pick new robber location
			// Request robber placement on map
				// If unsuccessful, place on random spot
		// Get players adjacent to robber hex
		// Give player option from those players who have a non-empty inventory to steal 1 random resource
		// Transfer 1 random resource from robber victim to player
	}
	// From player, listen for: Play Dev Card, Build Settlement, Build Road, Upgrade to City, Draw Dev Card, Trade
	// Play Dev Card
		// Check if player has this dev card. If so, run the dev card's behavior
		// Player can only do this action once per turn.
	// Build Settlement
		// Check if player can afford. If so, send build request to map. Send feedback to player whether build was successful.
		// (Map here handles player discount adjustment, probably)
		// Adjust VPs and Resource inventory accordingly.
		// Check for win
	// Upgrade to City
		// Check if player can afford. If so, send upgrade request to map. Send feedback to player whether upgrade was successful.
		// Adjust VPs and Resource inventory accordingly.
		// Check for win
	// Build Road
		// Check if player can afford. If so, send road build request to map. Send feedback to player whether build was successful.
		// Update player's longest road length
		// Check if player longest road is now bigger than current, if so assign VPs accordingly & check for win.
	// Draw Dev Card
		// Check if player can afford. If so, add random dev card from top of deck to player dev card inventory.
		// If VP, then adjust player VPs & check for win, card won't be playable.
		// Adjust Resource inventory accordingly.
	// Trade
		// Prompt player to pick resources to give and resources to take (both sides can't share resource types)
		// Prompt all other players to either: Accept trade, Decline trade, Send counteroffer
		// Prompt player with all other player responses. Player can either: Confirm a trade, Accept a counteroffer (which confirms it), or do nothing.
	// Player can do any of these actions an indefinite amount of times unless indicated otherwise (ideally under a time limit)
}

void Game::Start() {
	while (true) {
		for (Player& player : players) {
			PlayerTurn(player);
			if (player.victory_points >= vp_to_win) {
				// Double check (should be checked within PlayerTurn probably)
				Win(player);
				return;
			}
		}
		std::cin.get(); // temp: prevent inf loop
	}
}

void Game::Win(Player& player) {
	std::cout << player.name << " has won Catan!" << std::endl;
}

bool Game::BuildSettlement(Player& player, std::pair<int, int> pos, bool is_initial) {
	if (pos.first >= 0 && pos.first < map.node_grid.size() && pos.second >= 0 && pos.second < map.node_grid[pos.first].size()) {
		std::shared_ptr<Settlement> new_settle = std::make_shared<Settlement>(&player, &(map.node_grid[pos.first][pos.second].value()));
		if (map.PlaceOcc(new_settle, !is_initial)) {
			player.victory_points += 1;
			Catan_IO::Info(player.name + " built settlement at (" + std::to_string(pos.first) + "," + std::to_string(pos.second)
				+ ")! +1 VPs");
			return true;
		}
	}
	Catan_IO::Debug("Failed to build settlement for player '" + player.name + "' at (" + std::to_string(pos.first) + "," +
		std::to_string(pos.second) + ").");
	return false;
}

bool Game::BuildCity(Player& player, std::pair<int, int> pos) {
	if (pos.first >= 0 && pos.first < map.node_grid.size() && pos.second >= 0 && pos.second < map.node_grid[pos.first].size()) {
		std::shared_ptr<City> new_city = std::make_shared<City>(&player, &(map.node_grid[pos.first][pos.second].value()));
		if (map.ReplaceOcc(new_city)) {
			player.victory_points += 1;
			Catan_IO::Info(player.name + " built city at (" + std::to_string(pos.first) + "," + std::to_string(pos.second)
				+ ")! +1 VPs");
			return true;
		}
	}
	return false;
}

bool Game::BuildRoad(Player& player, std::pair<int, int> node_pos1, std::pair<int, int> node_pos2) {
	if (map.PlaceRoad(player, node_pos1, node_pos2)) {
		Catan_IO::Info(player.name + " built road from (" + std::to_string(node_pos1.first) + "," + std::to_string(node_pos1.second)
			+ ") to (" + std::to_string(node_pos2.first) + "," + std::to_string(node_pos2.second) + ")!");
		return true;
	}
	else {
		return false;
	}
}

void Game::UpdateLongestRoad() {
	Player* longest_road_holder = longest_road_plr;
	int longest_road = longest_road_holder ? longest_road_holder->longest_road : MIN_ROAD_SIZE - 1;
	for (Player& player : players) {
		if ((!longest_road_plr || player != *longest_road_plr) && player.longest_road > longest_road) {
			longest_road_holder = &player;
			longest_road = longest_road_holder->longest_road;
		}
	}
	if (longest_road_holder != longest_road_plr) {
		Catan_IO::Info(longest_road_holder->name + " has taken longest road! +2 VPs");
		if (longest_road_plr) {
			Catan_IO::Info(longest_road_plr->name + " has lost longest road! -2 VPs");
			longest_road_plr->victory_points -= 2;
		}
		longest_road_plr = longest_road_holder;
		longest_road_plr->victory_points += 2;
	}
}

void Game::UpdateLargestArmy() {
	Player* largest_army_holder = largest_army_plr;
	int largest_army = largest_army_holder ? largest_army_holder->army_size : MIN_ARMY_SIZE - 1;
	for (Player& player : players) {
		if ((!largest_army_plr || player != *largest_army_plr) && player.army_size > largest_army) {
			largest_army_holder = &player;
			largest_army = largest_army_holder->army_size;
		}
	}
	if (largest_army_holder != largest_army_plr) {
		if (largest_army_plr) {
			largest_army_plr->victory_points -= 2;
		}
		largest_army_plr = largest_army_holder;
		largest_army_plr->victory_points += 2;
	}
}

void Game::Dev_RB(Player& player) {
	Catan_IO::Info(player.name + " is playing Road Builder");
	for (int i = 0; i < 2; ++i) {
		std::unique_ptr<PositionDecisionResult>
			road_decision(dynamic_cast<PositionDecisionResult*>(player.Decide(game_state, Decision::POS_IniRoad).release()));
		if (!(road_decision && road_decision->positions.size() >= 2 &&
			BuildRoad(player, road_decision->positions[0], road_decision->positions[1]))) {
			std::optional<std::pair<std::pair<int, int>, std::pair<int, int>>> rand_spot = map.RoadGetRandPos(player);
			if (!(rand_spot && BuildRoad(player, rand_spot.value().first, rand_spot.value().second))) {
				Catan_IO::Info("No more road spots left!");
				return;
			}
		}
	}
}

void Game::Dev_Mono(Player& player) {
	Catan_IO::Info(player.name + " is playing Monopoly");
	std::unique_ptr<ResourceDecisionResult>
		mono_decision(dynamic_cast<ResourceDecisionResult*>(player.Decide(game_state, Decision::RSC_Monopoly).release()));
	Resource to_steal = mono_decision ? mono_decision->resources[0] : Resource::Lumber;
	int amnt = 0;
	for (Player& opponent : players) {
		if (opponent != player) {
			amnt += opponent.resources[to_steal];
			if (opponent.resources[to_steal] > 0) {
				Catan_IO::Info(player.name + " has stolen " + std::to_string(opponent.resources[to_steal]) +
					" " + RscToString(to_steal) + " from " + opponent.name + "!");
			}
			opponent.resources[to_steal] = 0;
		}
	}
	Catan_IO::Info(player.name + " stole a total of " + std::to_string(amnt) + " " + RscToString(to_steal) + "!");
	player.resources[to_steal] += amnt;
}