#pragma once

#include <utility>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cassert>
#include "resource.h"
#include "occupation.h"
#include "mapcomponents.h"


// All map interactions:
// Roll dice: 1: For each player, for each occupation, for each adj hex that is hit (without robber), give player resources if possible
//            2: For each hex that is hit (w/o robber), for each adj occupation, give player resources if possible
// Place settlement: Add a player settlement on specified node
// Place city: Add player city on specified node if player has a settlement there
// Place road: Pick two nodes. If nodes adj, player has either settlement on one of nodes or road linking to one of nodes, and nodes aren't already
//             linked, link them with this player.
// Place robber: Mark a hex (or hex position) as having robber placed on it.
// Print map: Get all map information: Hex locations, Occupation locations, Road locations

class Map {
public:
	std::vector<std::vector<std::optional<Hex>>> hex_grid;
	std::vector<std::vector<std::optional<Node>>> node_grid;
	std::pair<int, int> robber_pos;
private:
	/// <summary>
	/// Generates random Catan hex grid based on desired top row length and middle row length.
	/// Result generated inside of map.hex_grid
	/// </summary>
	/// <param name="top_row_len">- Length of top and bottom row of map</param>
	/// <param name="mid_row_len">- Length of center row of map</param>
	void GenerateHexes(int top_row_len, int mid_row_len);

	/// <summary>
	/// Generates a Catan node grid based on desired top row length and middle row length.
	/// Nodes represent locations where a settlement can be placed.
	/// Result generated inside of map.node_grid. 
	/// </summary>
	/// <param name="top_row_len">- Length of top and bottom row of map</param>
	/// <param name="mid_row_len">- Length of center row of map</param>
	void GenerateNodes(int top_row_len, int mid_row_len);

	/// <summary>
	/// Generates a random Catan map. Result is generated inside of map.hex_grid and map.node_grid
	/// </summary>
	/// <param name="top_row_len">- Length of top and bottom row of map</param>
	/// <param name="mid_row_len">- Length of center row of map</param>
	void Generate(int top_row_len, int mid_row_len);
public:
	/// <summary>
	/// Prints a representation of map.hex_grid to console
	/// </summary>
	void PrintHexes();

	/// <summary>
	/// Prints a representation of map.node_grid to console
	/// </summary>
	void PrintNodes();

	/// <summary>
	/// Produces a vector of pointers to all adjacent hexes to the Node located at (row, col) in map.node_grid
	/// </summary>
	/// <param name="row">- Node's row</param>
	/// <param name="col">- Node's column</param>
	/// <returns></returns>
	std::vector<Hex*> GetNodeHexes(int row, int col);

	/// <summary>
	/// Produces a vector of pointers to all adjacent nodes to the Hex located at (row, col) in map.hex_grid
	/// </summary>
	/// <param name="row">- Hex's row</param>
	/// <param name="col">- Hex's column</param>
	/// <returns></returns>
	std::vector<Node*> GetHexNodes(int row, int col);

	std::vector<std::pair<Resource, int>> GetResources(Occupation& occ);

	/// <summary>
	/// Constructs a Map instance. Generates a random map.
	/// </summary>
	Map();

	/// <summary>
	/// Constructs a Map instance. Generates a random map of desired size.
	/// </summary>
	/// <param name="top_row_len">- Length of top and bottom rows of map</param>
	/// <param name="mid_row_len">- Length of center row of map</param>
	Map(int top_row_len, int mid_row_len);
};


