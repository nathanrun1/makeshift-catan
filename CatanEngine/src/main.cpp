#include <iostream>
#include "game.h"

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
	map.node_grid[2][4]->occ.emplace(*map.node_grid[2][4]);
	std::vector<std::pair<Resource, int>> resources = map.GetResources(*map.node_grid[2][4]->occ);
	for (std::pair<Resource, int>& rsc_pair : resources) {
		std::cout << RscToString(rsc_pair.first) << " | " << rsc_pair.second << ";" << std::endl;
	}
}