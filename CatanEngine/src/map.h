#pragma once

#include <utility>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cassert>
#include "resource.h"

class Hex {
public:
	int dice_num;
	Resource resource;

	Hex(int dice_num, Resource resource)
		: dice_num(dice_num), resource(resource) {}
};

class Occupation {
public:
	int victory_points;
	int production;
};

class Link {};

class Node {
public:
	std::optional<Occupation> occ;
};

class Map {
public:
	std::vector<std::vector<std::optional<Hex>>> hex_grid;
	std::vector<std::vector<std::optional<Node>>> node_grid;
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
	// requires: top_row_len >= 0, mid_row_len >= top_row_len
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
	// time: O(1)
	std::vector<Hex*> GetNodeHexes(int row, int col);

	// GetHexNodes(row, col) produces a vector of pointers to all adjacent nodes to
	//   the hex at (row, col) in hex_grid
	// requires: hex_grid[row][col] exists
	// time: O(1)
	std::vector<Node*> GetHexNodes(int row, int col);

	Map() {
		Generate(3, 5);
	}
	Map(int top_row_len, int mid_row_len) {
		Generate(top_row_len, mid_row_len);
	}
};


