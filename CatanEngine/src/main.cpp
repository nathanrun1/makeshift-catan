#include <iostream>
#include "map.h"

// Progress:
// Map is able to generate grid of hexes and grid of nodes (spots where settlements go)
// Map uses 2D grid to represent hex positions and node positions
// Map can determine adjacent hexes to a node and vice versa

// Have to figure out how game will interact with map, and then adjust this accordingly, possibly
//   adding a position value to Nodes and Hexes themselves for easy checks


int main() {
	std::cout << "Hello Catan!" << std::endl;
	Map map;
	map.PrintHexes();
	std::vector<Hex*> my_hexes = map.GetNodeHexes(1, 1);
	for (Hex* hex : my_hexes) {
		std::cout << RscToString(hex->resource) << " " << hex->dice_num << "; ";
	}
	std::cout << std::endl;
}