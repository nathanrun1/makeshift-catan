#define _CATAN_DEBUG_
#include "game.h"

const std::unordered_map<Resource, int> Game::ROAD_COST = {
	{Resource::Brick, 1},
	{Resource::Lumber, 1}
};

const std::unordered_map<Resource, int> Game::SETTLE_COST = {
	{Resource::Brick, 1},
	{Resource::Lumber, 1},
	{Resource::Wool, 1},
	{Resource::Grain, 1}
};

const std::unordered_map<Resource, int> Game::CITY_COST = {
	{Resource::Ore, 3},
	{Resource::Grain, 2}
};

const std::unordered_map<Resource, int> Game::DEV_COST = {
	{Resource::Wool, 1},
	{Resource::Grain, 1},
	{Resource::Ore, 1}
};


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
	map.PrintHexes();
	Catan_IO::Info(player.name + "'s TURN");
	int dice_roll = DiceRoll();
	Catan_IO::Info("Dice rolled: " + std::to_string(dice_roll));
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
		}
		bank.ProcessMultiple(requests);
	}
	else {
		for (Player& p : players) {
			int rsc_amnt = p.GetResourceAmnt();
			if (rsc_amnt >= MIN_CARDS_DISCARD) {
				int to_discard = rsc_amnt / 2;
				Catan_IO::Info(p.name + ", you must discard " + std::to_string(to_discard) + " cards");
				std::unique_ptr<ResourceDecisionResult>
					discard_decision(dynamic_cast<ResourceDecisionResult*>(player.Decide(game_state, Decision::RSC_Discard).release()));
				int discarded = 0;
				std::vector<Resource> rscs_to_discard = discard_decision->resources;
				for (; discarded < std::min(to_discard, static_cast<int>(rscs_to_discard.size())); ++discarded) {
					if (p.resources[rscs_to_discard[discarded]] > 0) {
						p.resources[rscs_to_discard[discarded]] -= 1;
					}
					else {
						for (std::pair<const Resource, int>& rsc_amnt_pair : p.resources) {
							if (rsc_amnt_pair.second > 0) {
								p.resources[rsc_amnt_pair.first] -= 1;
								break;
							}
						}
					}
				}
				for (; discarded < to_discard; ++discarded) {
					// Decision didn't provide enough resources
					for (std::pair<const Resource, int>& rsc_amnt_pair : p.resources) {
						if (rsc_amnt_pair.second > 0) {
							p.resources[rsc_amnt_pair.first] -= 1;
							break;
						}
					}
				}
			}
		}
		PlayRobber(player);
	}
	// From player, listen for: Play Dev Card, Build Settlement, Build Road, Upgrade to City, Draw Dev Card, Trade
	bool dev_played = false;
	bool turn_over = false;
	while (!turn_over) {
		std::unique_ptr<OptionDecisionResult>
			turn_action(dynamic_cast<OptionDecisionResult*>(player.Decide(game_state, Decision::OPT_Turn).release()));
		int turn_action_choice = 0;
		if (turn_action) {
			turn_action_choice = turn_action->option;
		}
		switch (turn_action_choice) {
		case 0: {
			// Pass
			turn_over = true;
			break;
		}
		case 1: {
			// Trade
			// Add this and then game engine is done!!!
			std::unique_ptr<TradeDecisionResult>
				trade_decision(dynamic_cast<TradeDecisionResult*>(player.Decide(game_state, Decision::TRD_Trade).release()));
			std::queue<std::pair<Player*, Trade>> trade_queue;
			if (trade_decision) {
				Trade trade(trade_decision->offer, trade_decision->request);
				if (trade.is_valid() && CanAfford(player, trade.offer)) {
					std::unordered_map<Player*, Trade> accepted_trades;
					std::unordered_set<Trade> requested_trades;
					trade_queue.emplace(&player, trade);
					requested_trades.insert(trade);
					while (!trade_queue.empty()) {
						const std::pair<Player*, Trade> cur = trade_queue.front();
						trade_queue.pop();
						if (cur.first == &player) {
							Catan_IO::Info(cur.first->name + " is requesting a trade:\n" + static_cast<std::string>(cur.second));
						}
						else {
							Catan_IO::Info(cur.first->name + " has made a counter offer:\n" + static_cast<std::string>(cur.second));
						}
						for (Player& opponent : players) {
							if (opponent != player && opponent != *cur.first) {
								std::unique_ptr<OptionDecisionResult>
									trade_response_decision(dynamic_cast<OptionDecisionResult*>(
										opponent.Decide(game_state, player == *cur.first ? Decision::OPT_TradeRespond : Decision::OPT_TradeRespondCounter).release()));
								int response = 0;
								if (trade_response_decision) {
									response = trade_response_decision->option;
								}
								switch (response) {
								case 0: {
									// Decline
									break;
								}
								case 1: {
									// Accept
									if (CanAfford(opponent, cur.second.request)) {
										Catan_IO::Info(opponent.name + " has accepted the trade");
										accepted_trades[&opponent] = cur.second;
									}
									else {
										Catan_IO::Info("You cannot afford to accept this trade (Auto Declined)");
									}
									break;
								}
								case 2: {
									// Counter
									std::unique_ptr<TradeDecisionResult>
										counter_trade_decision(dynamic_cast<TradeDecisionResult*>(opponent.Decide(game_state, Decision::TRD_TradeCounter).release()));
									if (counter_trade_decision) {
										Trade counter_trade(counter_trade_decision->request, counter_trade_decision->offer);
										if (counter_trade.is_valid() && CanAfford(opponent, counter_trade.request)) {
											Catan_IO::Info(opponent.name + " has initiated a counter trade");
											accepted_trades[&opponent] = counter_trade;
											trade_queue.emplace(&opponent, counter_trade);
											requested_trades.insert(counter_trade);
										}
										else {
											Catan_IO::Info("Counter trade is invalid (Auto Declined)");
										}
									}
									break;
								}
								}
							}
						}
					}
					Catan_IO::Info("All responses received, options:");
					std::vector<std::pair<Player* const, Trade>> accepted_trades_vec;
					int option_cnt = 1;
					for (std::pair<Player* const, Trade>& plr_trade_pair : accepted_trades) {
						if (CanAfford(player, plr_trade_pair.second.offer)) {
							accepted_trades_vec.emplace_back(plr_trade_pair.first, plr_trade_pair.second);
							Catan_IO::Info("Option " + std::to_string(option_cnt) + ": " + plr_trade_pair.first->name +
								" accepts:\n" + static_cast<std::string>(plr_trade_pair.second));
							++option_cnt;
						}
					}
					std::unique_ptr<OptionDecisionResult>
						trade_responses_decision(dynamic_cast<OptionDecisionResult*>(player.Decide(game_state, Decision::OPT_TradeResponses).release()));
					int trade_responses_choice = 0;
					if (trade_responses_decision) {
						trade_responses_choice = trade_responses_decision->option;
					}
					switch (trade_responses_choice) {
					case 0:
						Catan_IO::Info(player.name + " chose not to finalize any trades");
						break;
					default:
						trade_responses_choice = std::clamp(trade_responses_choice, 1, static_cast<int>(accepted_trades_vec.size()));
						std::pair<Player* const, Trade> choice = accepted_trades_vec[trade_responses_choice - 1];
						for (std::pair<const Resource, int>& offered : choice.second.offer) {
							if (offered.second > 0) {
								player.resources[offered.first] -= offered.second;
								choice.first->resources[offered.first] += offered.second;
								Catan_IO::Info(player.name + " gave " + std::to_string(offered.second) +
									" " + RscToString(offered.first) + " to " + choice.first->name);
							}
						}
						for (std::pair<const Resource, int>& requested : choice.second.request) {
							if (requested.second > 0) {
								player.resources[requested.first] += requested.second;
								choice.first->resources[requested.first] -= requested.second;
								Catan_IO::Info(player.name + " took " + std::to_string(requested.second) +
									" " + RscToString(requested.first) + " from " + choice.first->name);
							}
						}
						break;
					}
				}
				else {
					Catan_IO::Info("Trade is invalid");
				}
			} 
			else {
				Catan_IO::Info("Trade is invalid");
			}
			if (player.victory_points > vp_to_win) {
				Win(player);
				return;
			}
			break;
		}
		case 2: {
			// Play Dev Card
			if (!dev_played) {
				std::unique_ptr<OptionDecisionResult>
					dev_decision(dynamic_cast<OptionDecisionResult*>(player.Decide(game_state, Decision::OPT_PlayDev).release()));
				int dev_choice = 0;
				if (dev_decision) {
					dev_choice = dev_decision->option;
				}
				DevCard to_play;
				switch (dev_choice) {
				case 0:
					// Knight
					to_play = DevCard::Knight;
					break;
				case 1:
					// YOP
					to_play = DevCard::YearOfPlenty;
					break;
				case 2:
					// Road Builder
					to_play = DevCard::RoadBuilder;
					break;
				case 3:
					// Monopoly
					to_play = DevCard::Monopoly;
					break;
				default:
					// Default to Knight, arbitrary
					to_play = DevCard::Knight;
					break;
				}
				bool has_card = false;
				for (std::vector<DevCard>::iterator dev_it = player.dev_cards.begin(); dev_it != player.dev_cards.end(); dev_it++) {
					if (*dev_it == to_play) {
						has_card = true;
						player.dev_cards.erase(dev_it);
						break;
					}
				}
				if (has_card) {
					dev_played = true;
					PlayDev(player, to_play);
				}
				else {
					Catan_IO::Info("You do not have this dev card");
				}
			}
			else {
				Catan_IO::Info("You can only play a dev card once per turn");
			}
			if (player.victory_points > vp_to_win) {
				Win(player);
				return;
			}
			break;
		}
		case 3: {
			// Draw Dev Card
			if (CanAfford(player, DEV_COST)) {
				std::optional<DevCard> draw = map.DrawDevcard();
				if (draw != std::nullopt) {
					player.dev_cards.push_back(draw.value());
					Catan_IO::Info(player.name + " drew a dev card");
					Catan_IO::Debug("Dev card: " + devcard_str_map.at(draw.value()));
					Purchase(player, DEV_COST);
					if (draw.value() == DevCard::VictoryPoint) {
						player.victory_points += 1;
					}
				}
				else {
					Catan_IO::Info("No dev cards left!");
				}
			}
			else {
				Catan_IO::Info("You cannot afford to draw a dev card.");
			}
			if (player.victory_points > vp_to_win) {
				Win(player);
				return;
			}
			break;
		}
		case 4: {
			// Build road
			if (CanAfford(player, ROAD_COST)) {
				std::unique_ptr<PositionDecisionResult>
					road_decision(dynamic_cast<PositionDecisionResult*>(player.Decide(game_state, Decision::POS_BuildRoad).release()));
				if (!(road_decision && road_decision->positions.size() >= 2 &&
					BuildRoad(player, road_decision->positions[0], road_decision->positions[1]))) {
					std::optional<std::pair<std::pair<int, int>, std::pair<int, int>>> rand_spot = map.RoadGetRandPos(player);
					if (!(rand_spot && BuildRoad(player, rand_spot.value().first, rand_spot.value().second))) {
						Catan_IO::Info("No more road spots left!");
					}
					else {
						Purchase(player, ROAD_COST);
					}
				}
				else {
					Purchase(player, ROAD_COST);
				}
			}
			else {
				Catan_IO::Info("You cannot afford to build a road");
			}
			if (player.victory_points > vp_to_win) {
				Win(player);
				return;
			}
			break;
		}
		case 5: {
			// Settle
			if (CanAfford(player, SETTLE_COST)) {
				std::unique_ptr<PositionDecisionResult>
					settle_decision(dynamic_cast<PositionDecisionResult*>(player.Decide(game_state, Decision::POS_Settle).release()));
				std::pair<int, int> settle_pos(0, 0);
				if (!(settle_decision && settle_decision->positions.size() > 0 && BuildSettlement(player, settle_pos = settle_decision->positions[0]))) {
					std::optional<std::pair<int, int>> rand_pos_opt;
					if (!((rand_pos_opt = map.OccGetRandPos(player)) != std::nullopt && BuildSettlement(player, settle_pos = rand_pos_opt.value()))) {
						Catan_IO::Info("No valid spots available!");
					}
					else {
						Purchase(player, SETTLE_COST);
					}
				}
				else {
					Purchase(player, SETTLE_COST);
				}
			}
			else {
				Catan_IO::Info("You cannot afford to build a settlement");
			}
			if (player.victory_points > vp_to_win) {
				Win(player);
				return;
			}
			break;
		}
		case 6: {
			// City
			if (CanAfford(player, CITY_COST)) {
				std::unique_ptr<PositionDecisionResult>
					city_decision(dynamic_cast<PositionDecisionResult*>(player.Decide(game_state, Decision::POS_City).release()));
				if (city_decision && city_decision->positions.size() > 0 && BuildCity(player, city_decision->positions[0])) {
					Purchase(player, CITY_COST);
				}
				else {
					Catan_IO::Info("Invalid city location");
				}
			}
			else {
				Catan_IO::Info("You cannot afford to build a city");
			}
			if (player.victory_points > vp_to_win) {
				Win(player);
				return;
			}
			break;
		}
		case 7: {
			// Bank Trade

			// Prompt for trade decision with bank
			// If: trade matches player's discounts, is a valid bank trade and bank has enough resources:
			// Run the trade with the bank

			// To check if matches discounts, just match up the ratio of each discount with each resource in the trade with
			// the amount of resources requested from bank
			// If its an exact match, then ok, otherwise its invalid
		}
		}
	}
}

void Game::Start() {
	GetPlayers();
	SetupPhase();
	while (true) {
		for (Player& player : players) {
			PlayerTurn(player);
			if (player.victory_points >= vp_to_win) {
				Win(player);
				return;
			}
		}
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
		if (longest_road_plr != nullptr && player.longest_road > longest_road_plr->longest_road) {
			UpdateLongestRoad();
		}
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
			road_decision(dynamic_cast<PositionDecisionResult*>(player.Decide(game_state, Decision::POS_BuildRoad).release()));
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
	Resource to_steal = (mono_decision != nullptr && mono_decision->resources.size() > 0) ? mono_decision->resources[0] : Resource::Lumber;
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

void Game::Dev_YOP(Player& player) {
	Catan_IO::Info(player.name + " is playing Year of Plenty");
	std::unique_ptr<ResourceDecisionResult>
		yop_decision(dynamic_cast<ResourceDecisionResult*>(player.Decide(game_state, Decision::RSC_YOP).release()));
	if (yop_decision != nullptr && yop_decision->resources.size() == 2) {
		for (const Resource& requested : yop_decision->resources) {
			Bank::Request request(&player, requested, 1);
			if (!bank.Process(request)) {
				std::optional<Resource> alt_rsc = std::nullopt;
				for (std::pair<const Resource, int>& rsc : bank.inventory) {
					if (rsc.second > 0) {
						alt_rsc.emplace(rsc.first);
						break;
					}
				}
				if (alt_rsc) {
					Catan_IO::Debug("Resource not available, defaulting to " + RscToString(alt_rsc.value()));
					Bank::Request new_req(&player, alt_rsc.value(), 1);
					bank.Process(new_req);
				}
				else {
					Catan_IO::Info("Bank is empty!");
					return;
				}
			}
		}
	}
	else {
		Catan_IO::Debug("Invalid input, picking up two available resources.");
		for (int i = 0; i < 2; ++i) {
			std::optional<Resource> alt_rsc = std::nullopt;
			for (std::pair<const Resource, int>& rsc : bank.inventory) {
				if (rsc.second > 0) {
					alt_rsc.emplace(rsc.first);
					break;
				}
			}
			if (alt_rsc) {
				Bank::Request new_req(&player, alt_rsc.value(), 1);
				bank.Process(new_req);
			}
			else {
				Catan_IO::Info("Bank is empty!");
				return;
			}
		}
	}
}

void Game::Dev_Knight(Player& player) {
	Catan_IO::Info(player.name + " is playing Knight");
	PlayRobber(player);
	player.army_size += 1;
	if (largest_army_plr != nullptr && player.army_size > largest_army_plr->army_size) {
		UpdateLargestArmy();
	}
}

void Game::PlayDev(Player& player, DevCard dev_card) {
	switch (dev_card) {
	case DevCard::Knight:
		Dev_Knight(player);
		break;
	case DevCard::YearOfPlenty:
		Dev_YOP(player);
		break;
	case DevCard::RoadBuilder:
		Dev_RB(player);
		break;
	case DevCard::Monopoly:
		Dev_Mono(player);
		break;
	}
}

void Game::PlayRobber(Player& player) {
	std::unique_ptr<PositionDecisionResult>
		robber_decision(dynamic_cast<PositionDecisionResult*>(player.Decide(game_state, Decision::POS_Robber).release()));
	std::pair<int, int> new_robber_pos;
	std::optional<std::vector<Player*>> result;
	std::vector<Player*> adj_players;
	if (robber_decision != nullptr && robber_decision->positions.size() > 0) {
		new_robber_pos = robber_decision->positions[0];
		if ((result = map.PlaceRobber(new_robber_pos)) == std::nullopt) {
			if (new_robber_pos != std::pair<int, int>(0, 0)) {
				new_robber_pos = std::pair<int, int>(0, 0);
			}
			else {
				new_robber_pos = std::pair<int, int>(0, 1);
			}
			result = map.PlaceRobber(new_robber_pos);
		}
	}
	else if (!(result = map.PlaceRobber(new_robber_pos = { 0, 0 }))) {
		result = map.PlaceRobber(new_robber_pos = { 0, 1 });
	}
	adj_players = result.value();
	Catan_IO::Info(player.name + " placed robber on (" + std::to_string(new_robber_pos.first) + "," +
		std::to_string(new_robber_pos.second) + ")!");
	std::vector<Player*> steal_options;
	for (Player*& adj_player : adj_players) {
		if (*adj_player != player && adj_player->GetResourceAmnt() > 0) {
			steal_options.push_back(adj_player);
		}
	}
	Player* victim = nullptr;
	if (steal_options.size() > 0) {
		if (steal_options.size() == 1) {
			victim = steal_options[0];
		}
		else {
			Catan_IO::Info("Choose from following players to steal from:");
			int cnt = 0;
			for (std::vector<Player*>::iterator opt_it = steal_options.begin(); opt_it != steal_options.end(); opt_it++) {
				Catan_IO::Info("Option " + std::to_string(cnt) + ": " + (*opt_it)->name); // fix
				cnt++;
			}
			std::unique_ptr<OptionDecisionResult>
				steal_decision(dynamic_cast<OptionDecisionResult*>(player.Decide(game_state, Decision::OPT_Steal).release()));
			if (steal_decision) {
				victim = steal_options[std::clamp(static_cast<int>(steal_decision->option), 0, static_cast<int>(steal_options.size() - 1))];
			}
			else {
				victim = steal_options[0];
			}
		}
		std::discrete_distribution<> distr{ {
				static_cast<double>(victim->resources[Resource::Lumber]),
				static_cast<double>(victim->resources[Resource::Brick]),
				static_cast<double>(victim->resources[Resource::Ore]),
				static_cast<double>(victim->resources[Resource::Grain]),
				static_cast<double>(victim->resources[Resource::Wool])
		} };
		int rsc_to_steal_int = distr(m_gen);
		Resource rsc_to_steal;
		switch (rsc_to_steal_int) {
		case 0:
			rsc_to_steal = Resource::Lumber;
			break;
		case 1:
			rsc_to_steal = Resource::Brick;
			break;
		case 2:
			rsc_to_steal = Resource::Ore;
			break;
		case 3:
			rsc_to_steal = Resource::Grain;
			break;
		case 4:
			rsc_to_steal = Resource::Wool;
			break;
		}
		victim->resources[rsc_to_steal] -= 1;
		player.resources[rsc_to_steal] += 1;
		Catan_IO::Info(player.name + " stole a random resource from " + victim->name);
		Catan_IO::Debug("Rsc stolen: " + RscToString(rsc_to_steal));
	}
	else {
		Catan_IO::Info("No players to steal from");
	}
}

bool Game::CanAfford(Player& player, const std::unordered_map<Resource, int>& cost) {
	bool can_afford = true;
	for (const std::pair<const Resource, int>& rsc_cost : cost) {
		if (player.resources[rsc_cost.first] < rsc_cost.second) {
			can_afford = false;
			break;
		}
	}
	return can_afford;
}

void Game::Purchase(Player& player, const std::unordered_map<Resource, int>& cost) {
	for (const std::pair<const Resource, int>& rsc_cost : cost) {
		player.resources[rsc_cost.first] -= rsc_cost.second;
		Catan_IO::Info(player.name + " gave " + std::to_string(rsc_cost.second) + " " + RscToString(rsc_cost.first) + " to bank");
	}
}