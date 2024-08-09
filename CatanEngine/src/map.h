#pragma once

#include <utility>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cassert>
#include "resource.h"
#include "player.h"


// All map interactions:
// Roll dice: 1: For each player, for each occupation, for each adj hex that is hit (without robber), give player resources if possible
//            2: For each hex that is hit (w/o robber), for each adj occupation, give player resources if possible
// Place settlement: Add a player settlement on specified node
// Place city: Add player city on specified node if player has a settlement there
// Place road: Pick two nodes. If nodes adj, player has either settlement on one of nodes or road linking to one of nodes, and nodes aren't already
//             linked, link them with this player.
// Place robber: Mark a hex (or hex position) as having robber placed on it.
// Print map: Get all map information: Hex locations, Occupation locations, Road locations
class Node;

class Hex {
public:
	int dice_num;
	Resource resource; 

	Hex(int dice_num, Resource resource)
		: dice_num(dice_num), resource(resource) {}
};

class Occupation {
public:
	//Node node;

	//Occupation(Node node) : node(node) {}
};

//class Settlement : Occupation {
//public:
//	int victory_points = 1;
//	int production = 1;
//	Player player;
//
//	Settlement(Player player, Node node) : player(player), Occupation(node) {}
//};

//class City : Settlement {
//public:
//	int victory_points = 2;
//	int production = 2;
//
//	City(Player player, Node node) : Settlement(player, node) {}
//};

class Node {
public:
	Occupation* occ;
	std::vector<Hex*> adj_hexes;
	std::vector<std::pair<Node*, Player*>> roads;
}; // fix

class Map {
public:
	std::vector<std::vector<std::optional<Hex>>> hex_grid;
	std::vector<std::vector<std::optional<Node>>> node_grid;
	std::pair<int, int> robber_pos;
private:
	// GenerateHexes(top_row_len, mid_row_len) generates a random Catan hex grid based on desired
	//   top row length and middle row length. Grid is generated inside of hex_grid.
	// requires: top_row_len >= 0, mid_row_len >= top_row_len
	// effects: mutates hex_grid
	// time: O(n^2) where n = mid_row_len
	void GenerateHexes(int top_row_len, int mid_row_len);

	// GenerateNodes(top_row_len, mid_row_len) generates a Catan node grid based on desired
	//   top row length and middle row length. Grid is generated inside of node_grid. Generated node grid
	//   coincides with a hex grid generated with the same dimensions.
	// requires: top_row_len >= 0, mid_row_len >= top_row_len, 
	//           hex_grid has already been generated and has same dimensions
	// effects: mutates node_grid
	// time: O(n^2) where n = mid_row_len
	void GenerateNodes(int top_row_len, int mid_row_len);

	// Generate(top_row_len, mid_row_len) generates a random Catan map. Map is generated inside of
	//   hex_grid and node_grid.
	// requires: top_row_len >= 0, mid_row_len >= top_row_len
	// effects: mutates hex_grid, mutates node_grid
	// time: O(n^2) where n = mid_row_len
	void Generate(int top_row_len, int mid_row_len);
public:
	// PrintHexes() prints hex_grid to console.
	// effects: writes to console
	// time: O(n) where n is amount of hexes in hex_grid
	void PrintHexes();

	// PrintNodes() prints node_grid to console.
	// effects: writes to console
	// time: O(n) where n is amount of nodes in node_grid
	void PrintNodes();

	// GetNodeHexes(row, col) produces a vector of pointers to all adjacent hexes to
	//   the node at (row, col) in node_grid
	// requires: node_grid[row][col] exists
	//           hex_grid has already been generated with same dimensions as node_grid
	// time: O(1)
	std::vector<Hex*> GetNodeHexes(int row, int col);

	// GetHexNodes(row, col) produces a vector of pointers to all adjacent nodes to
	//   the hex at (row, col) in hex_grid
	// requires: hex_grid[row][col] exists
	//           node_grid has already been generated with same dimensions as hex_grid
	// time: O(1)
	std::vector<Node*> GetHexNodes(int row, int col);

	Map() {
		Generate(3, 5);
	}
	Map(int top_row_len, int mid_row_len) {
		Generate(top_row_len, mid_row_len);
	}
};


