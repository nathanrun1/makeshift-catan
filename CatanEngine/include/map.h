#pragma once

#include <utility>
#include <vector>
#include <optional>
#include <unordered_map>
#include <cassert>
#include "mapcomponents.h"
#include "pair_hash.h"


class Map {
public:
	static const std::vector<Resource> MAP_RESOURCES;
	static const std::vector<int> DICE_NUMBERS;
	static const std::vector<std::vector<std::pair<Resource, int>>> PORT_DISCOUNTS;
	static const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> PORT_LOCATIONS;
	std::random_device seed_gen;
	std::mt19937 gen;

	std::vector<std::vector<std::optional<Hex>>> hex_grid;
	std::vector<std::vector<std::optional<Node>>> node_grid;
	std::unordered_map<Node*, std::vector<std::pair<Node*, Player*>>> roads;
	std::unordered_map<std::pair<int, int>, std::vector<std::pair<Resource, int>>> port_map;
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
	/// Generates port locations by shuffling the port types around all possible port locations. 
	/// Result generated inside of map.port_map
	/// </summary>
	void GeneratePorts();

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

	/// <summary>
	/// Produces a vector of pointers to all adjacent hexes to the Node located at (row, col) in map.node_grid
	/// </summary>
	/// <param name="node_pos">- Position of given node </param>
	std::vector<Hex*> GetNodeHexes(std::pair<int, int> node_pos);

	/// <summary>
	/// Produces a vector of pointers to all adjacent nodes to the Hex located at (row, col) in map.hex_grid
	/// </summary>
	/// <param name="hex_pos">- Position of given hex </param>
	std::vector<Node*> GetHexNodes(std::pair<int, int> hex_pos);

	/// <summary>
	/// Produces a vector of pointers to all adjacent nodes to the node at position node_pos
	/// </summary>
	/// <param name="node_pos">- Position of given node</param>
	std::vector<Node*> GetAdjNodes(std::pair<int, int> node_pos);

	/// <summary>
	/// Produces a vector of pointers to all nodes that a player has a road connecting to.
	/// Assumes that a player needs at least one settlement to have any roads.
	/// </summary>
	std::vector<Node*> GetPlayerNodes(Player& player);
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
	/// Returns a list of available resources to an Occupation.
	/// </summary>
	/// <param name="occ">- Occupation being checked</param>
	/// <returns></returns>
	std::vector<std::pair<Resource, int>> GetResources(Occupation& occ);

	/// <summary>
	/// Attempt to place robber at given position.
	/// </summary>
	/// <param name="pos">- Position of hex on which to place robber</param>
	/// <returns>
	/// If successful, a vector of pointers to players that have occupations adjacent to the robber.
	/// Otherwise, returns std::nullopt. 
	/// </returns>
	std::optional<std::vector<Player*>> PlaceRobber(std::pair<int, int> pos);

	/// <summary>
	/// Attempt to place a road between two nodes at given positions.
	/// Both nodes must be adjacent.
	/// One of the two nodes needs to either have an occupation belonging to the player,
	/// or have an attached road belonging to the player. A road also must not already exist
	/// in the same location.
	/// Updates player's longest road on successful placement.
	/// </summary>
	/// <param name="player">- Player placing the road</param>
	/// <param name="node_pos1">- First node position</param>
	/// <param name="node_pos2">- Second node position</param>
	/// <returns>True if successful, false otherwise</returns>
	bool PlaceRoad(Player& player, std::pair<int, int> node_pos1, std::pair<int, int> node_pos2);

	/// <summary>
	/// Checks if a road exists between two provided node positions. Assumes that 
	/// provided positions are valid.
	/// </summary>
	/// <returns>True if road exists, false if spot is available</returns>
	bool RoadExists(std::pair<int, int> node_pos1, std::pair<int, int> node_pos2);


	/// <summary>
	/// Attempt to place an occupation (Settlement/City) on the map. If successful, adds the occupation to the 
	/// associated Player's occupation list.
	/// </summary>
	/// <param name="occ">- Occupation to be placed</param>
	/// <param name="needs_road">- Whether or not an adjacent road needs to exist to validate placement</param>
	/// <returns></returns>
	bool PlaceOcc(std::shared_ptr<Occupation> occ, bool needs_road = true);

	/// <summary>
	/// Gets a random valid occupation spot for a given player
	/// </summary>
	/// <param name="player">- Player placing the occupation</param>
	/// <param name="needs_road">- Whether or not an adjacent road needs to exist to validate placement</param>
	/// <returns>Valid position if exists, otherwise std::nullopt</returns>
	std::optional<std::pair<int, int>> OccGetRandPos(Player& player, bool needs_road = true);

	/// <summary>
	/// Gets a random valid spot for road placement for player.
	/// If occupation given, will limit valid spots to adjacent to given occupation.
	/// Otherwise, will search all possible locations.
	/// </summary>
	/// <param name="player">- Player placing the road</param>
	/// <param name="occ">- Occupation that the road must be adjacent to (optional)</param>
	/// <returns>- Pair of node positions, road spot is between two provided nodes, nullopt if no valid position exists.</returns>
	std::optional<std::pair<std::pair<int, int>, std::pair<int, int>>> RoadGetRandPos(Player& player, std::optional<std::shared_ptr<Occupation>> occ = std::nullopt);

	/// <summary>
	/// Attempt to replace an already existing occupation with a new one .
	/// Occupation must already exist at same position.
	/// New occupation owner must be same as previous.
	/// </summary>
	/// <param name="occ">- Occupation to replace with</param>
	/// <returns></returns>
	bool ReplaceOcc(std::shared_ptr<Occupation> occ);

	/// <summary>
	/// Determines the given player's longest road on the map.
	/// </summary>
	/// <returns>Length of player's longest road</returns>
	int GetLongestRoad(Player& player);

	/// <summary>
	/// Constructs a Map instance. Generates a random map.
	/// </summary>
	Map();

	/// <summary>
	/// Constructs a Map instance. Generates a random map using given seed;
	/// </summary>
	/// <param name="seed"></param>
	Map(const unsigned int seed);
};


