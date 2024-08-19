#define LOG(x) std::cout << x << std::endl

#include "catan_io.h"
#include <iostream>
#include <memory>
#include <chrono>
#include "game.h"
#include "buildings.h"




// Progress:
// Map is able to generate grid of hexes and grid of nodes (spots where settlements go)
// Map uses 2D grid to represent hex positions and node positions
// Map can determine adjacent hexes to a node and vice versa

// Have to figure out how game will interact with map, and then adjust this accordingly, possibly
//   adding a position value to Nodes and Hexes themselves for easy checks

int main() {
	/*std::cout << "Hello Catan!" << std::endl;
	Map map;
	map.PrintHexes();
	std::vector<Hex*> my_hexes = map.node_grid[1][3]->adj_hexes;
	for (Hex* hex : my_hexes) {
		std::cout << RscToString(hex->resource) << " " << hex->dice_num << "; ";
	}
	std::cout << std::endl;
	std::cin.get();*/

	/*Request request(Resource::Wool, 4);
	Bank bank;
	std::cout << bank.RequestResource(request);
	std::cout << bank.RequestResource(request);
	std::cout << bank.RequestResource(request);
	std::cout << bank.RequestResource(request);
	std::cout << static_cast<std::string>(bank);*/

	/*Game game;
	std::cout << game.DiceRoll() << "\n";
	Player plr("Nathan");*/

	//Map map;
	/*map.PrintHexes();
	Player plr("Nathan");
	std::shared_ptr<Settlement> my_settlement = std::make_shared<Settlement>(&plr, &map.node_grid[2][4].value());
	map.PlaceOcc(my_settlement, false);
	std::optional<std::vector<Player*>> adj_players = map.PlaceRobber(std::pair<int, int>(2, 2));
	if (adj_players) {
		for (Player*& plr : *adj_players) {
			std::cout << plr->name << std::endl;
		}
	}
	std::cout << map.robber_pos.first << " " << map.robber_pos.second << std::endl;
	std::vector<std::pair<Resource, int>> resources = map.GetResources(*my_settlement);
	std::cout << RscToString(map.hex_grid[2][2]->resource) << std::endl;
	for (std::pair<Resource, int>& rsc_pair : resources) {
		std::cout << RscToString(rsc_pair.first) << " | " << rsc_pair.second << ";" << std::endl;
	}*/
	/*for (const std::pair<std::pair<int, int>, std::pair<int, int>>& port_location : Map::PORT_LOCATIONS) {
		std::vector<std::pair<Resource, int>> discount = map.port_map[port_location.first];
		LOG("Port location (" << port_location.first.first << ", " << port_location.first.second << ")");
		for (std::pair<Resource, int>& d : discount) {
			LOG("    " << RscToString(d.first) << " " << d.second);
		}
	}*/

	/*Player plr("Nathan");
	std::shared_ptr<Settlement> port_village = std::make_shared<Settlement>(&plr, &map.node_grid[0][0].value());
	map.PlaceOcc(port_village, false);

	LOG("Expected discount(s):");
	for (std::pair<Resource, int>& d : map.port_map[std::pair<int, int>(0, 0)]) {
		LOG("    " << RscToString(d.first) << " " << d.second);
	}

	LOG("Player discounts:");
	for (std::pair<const Resource, int>& d : plr.discounts) {
		LOG("    " << RscToString(d.first) << " " << d.second);
	}*/

	//Player plr("Nathan");
	//std::shared_ptr<Settlement> my_settlement = std::make_shared<Settlement>(&plr, &map.node_grid[3][2].value());
	//std::shared_ptr<Settlement> my_settlement2 = std::make_shared<Settlement>(&plr, &map.node_grid[3][5].value());
	//std::shared_ptr<City> my_city = std::make_shared<City>(&plr, &map.node_grid[3][2].value());

	//map.PlaceOcc(my_settlement, false);
	////map.PlaceOcc(my_settlement2, false);
	//map.ReplaceOcc(my_city);
	/*map.PlaceRoad(plr, { 3, 2 }, { 3, 3 });
	map.PlaceRoad(plr, { 3, 3 }, { 3, 4 });
	map.PlaceRoad(plr, { 3, 4 }, { 4, 5 });
	map.PlaceRoad(plr, { 4, 5 }, { 4, 4 });
	map.PlaceRoad(plr, { 4, 4 }, { 5, 5 });
	map.PlaceRoad(plr, { 5, 5 }, { 5, 6 });
	map.PlaceRoad(plr, { 3, 4 }, { 3, 5 });
	map.PlaceRoad(plr, { 3, 5 }, { 3, 6 });
	map.PlaceRoad(plr, { 3, 6 }, { 3, 7 });
	map.PlaceRoad(plr, { 3, 7 }, { 3, 8 });
	map.PlaceRoad(plr, { 3, 8 }, { 4, 9 });
	map.PlaceRoad(plr, { 4, 9 }, { 4, 10 });
	map.PlaceRoad(plr, { 4, 10 }, { 4, 11 });
	map.PlaceRoad(plr, { 3, 5 }, { 2, 4 });
	map.PlaceRoad(plr, { 2, 4 }, { 2, 3 });
	map.PlaceRoad(plr, { 2, 4 }, { 2, 5 });
	map.PlaceRoad(plr, { 2, 5 }, { 1, 4 });
	map.PlaceRoad(plr, { 1, 4 }, { 1, 3 });
	map.PlaceRoad(plr, { 1, 4 }, { 1, 5 });
	map.PlaceRoad(plr, { 2, 5 }, { 2, 6 });
	map.PlaceRoad(plr, { 2, 6 }, { 3, 7 });
	map.PlaceRoad(plr, { 2, 6 }, { 2, 7 });
	map.PlaceRoad(plr, { 2, 7 }, { 2, 8 });
	map.PlaceRoad(plr, { 2, 8 }, { 3, 9 });
	map.PlaceRoad(plr, { 3, 9 }, { 3, 10 });
	map.PlaceRoad(plr, { 3, 10 }, { 4, 11 });
	map.PlaceRoad(plr, { 3, 10 }, { 3, 11 });
	map.PlaceRoad(plr, { 3, 11 }, { 2, 10 });*/

	/*Player plr("Nathan");
	GameState game_state = std::bitset<32>();*/
	/*std::unique_ptr<TradeDecisionResult> decision(dynamic_cast<TradeDecisionResult*>(plr.Decide(game_state, Decision::TRD_Trade).release()));
	std::cout << "Offering: "
		<< decision->offer.at(Resource::Lumber) << " Lumber, "
		<< decision->offer.at(Resource::Brick) << " Brick, "
		<< decision->offer.at(Resource::Ore) << " Ore, "
		<< decision->offer.at(Resource::Grain) << " Grain, "
		<< decision->offer.at(Resource::Wool) << " Wool" << " | Requesting: "
		<< decision->request.at(Resource::Lumber) << " Lumber, "
		<< decision->request.at(Resource::Brick) << " Brick, "
		<< decision->request.at(Resource::Ore) << " Ore, "
		<< decision->request.at(Resource::Grain) << " Grain, "
		<< decision->request.at(Resource::Wool) << " Wool!";*/

	/*Game game;
	game.SetupTurn(plr);
	std::cout << plr.occupations[0]->node->pos.first << " " << plr.occupations[0]->node->pos.second << "\n";
	std::cout << "Roads:\n";
	for (std::pair<Node*, Player*>& road : game.map.roads[plr.occupations[0]->node]) {
		std::cout << plr.occupations[0]->node->pos.first << " " << plr.occupations[0]->node->pos.second
			<< " -> " << road.first->pos.first << " " << road.first->pos.second << "\n";
	}*/

	Game game;
	/*game.GetPlayers();
	game.SetupPhase();
	game.players[0].resources[Resource::Brick] = 3;
	Catan_IO::Debug("Giving " + game.players[0].name + " 3 Lumber");
	game.players[1].resources[Resource::Lumber] = 5;
	Catan_IO::Debug("Giving " + game.players[1].name + " 5 Lumber");
	game.Dev_Knight(game.players[2]);*/

	// next to do
	// functionality for knight and YOP (include edge cases where no resources left in bank!
	// make sure that resource giving checks if robber is on hex (pre sure it does)
	// program rest of game functionality including dev cards
	// almost fucking done holy shit

	game.Start();
}