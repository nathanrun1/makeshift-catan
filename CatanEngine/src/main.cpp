#include <iostream>
#include <memory>
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

	Map map;
	map.PrintHexes();
	Player plr(0, "Nathan");
	std::shared_ptr<Settlement> my_settlement = std::make_shared<Settlement>(&plr, &map.node_grid[2][4].value());
	map.PlaceOcc(my_settlement, false);
	map.PlaceRobber(std::pair<int, int>(2, 2));
	std::cout << map.robber_pos.first << " " << map.robber_pos.second << std::endl;
	std::vector<std::pair<Resource, int>> resources = map.GetResources(*my_settlement);

	std::shared_ptr<Settlement> settlement_again = std::dynamic_pointer_cast<Settlement>(map.node_grid[2][4].value().occ);
	std::cout << settlement_again->production << std::endl;


	std::cout << RscToString(map.hex_grid[2][2]->resource) << std::endl;
	for (std::pair<Resource, int>& rsc_pair : resources) {
		std::cout << RscToString(rsc_pair.first) << " | " << rsc_pair.second << ";" << std::endl;
	}
	std::cin.get();
}