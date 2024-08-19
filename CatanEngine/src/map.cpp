#include <iostream>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stack>
#include "map.h"

#define LOG(x) std::cout << x << std::endl

Map::Map() : gen(seed_gen()) {
	Generate(3, 5);
}

Map::Map(const unsigned int seed) : gen(seed) {
	Generate(3, 5);
}

//Map::Map(int top_row_len, int mid_row_len) {
//	Generate(top_row_len, mid_row_len);
//}

const std::vector<Resource> Map::MAP_RESOURCES = {
	Resource::Ore, Resource::Ore, Resource::Ore,
	Resource::Wool, Resource::Wool, Resource::Wool, Resource::Wool,
	Resource::Grain, Resource::Grain, Resource::Grain, Resource::Grain,
	Resource::Brick, Resource::Brick, Resource::Brick,
	Resource::Lumber, Resource::Lumber, Resource::Lumber, Resource::Lumber,
	Resource::Desert
};

const std::vector<int> Map::DICE_NUMBERS = {
	2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12
};

const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> Map::PORT_LOCATIONS = {
	{{0, 0}, {0, 1}},
	{{0, 3}, {0, 4}},
	{{1, 0}, {2, 1}},
	{{1, 7}, {1, 8}},
	{{2, 10}, {3, 11}},
	{{3, 2}, {4, 3}},
	{{4, 10}, {4, 11}},
	{{5, 5}, {5, 6}},
	{{5, 8}, {5, 9}}
};

const std::vector<std::vector<std::pair<Resource, int>>> Map::PORT_DISCOUNTS = {
	{{Resource::Brick, 2}},
	{{Resource::Lumber, 2}},
	{{Resource::Grain, 2}},
	{{Resource::Ore, 2}},
	{{Resource::Wool, 2}},
	{{Resource::Brick, 3}, {Resource::Lumber, 3}, {Resource::Grain, 3}, {Resource::Ore, 3}, {Resource::Wool, 3}},
	{{Resource::Brick, 3}, {Resource::Lumber, 3}, {Resource::Grain, 3}, {Resource::Ore, 3}, {Resource::Wool, 3}},
	{{Resource::Brick, 3}, {Resource::Lumber, 3}, {Resource::Grain, 3}, {Resource::Ore, 3}, {Resource::Wool, 3}},
	{{Resource::Brick, 3}, {Resource::Lumber, 3}, {Resource::Grain, 3}, {Resource::Ore, 3}, {Resource::Wool, 3}}
};

const std::vector<DevCard> Map::DEVCARDS = {
		DevCard::Knight,DevCard::Knight,DevCard::Knight,DevCard::Knight,DevCard::Knight,
		DevCard::Knight,DevCard::Knight,DevCard::Knight,DevCard::Knight,DevCard::Knight,
		DevCard::Knight,DevCard::Knight,DevCard::Knight,DevCard::Knight,
		DevCard::VictoryPoint,DevCard::VictoryPoint,DevCard::VictoryPoint,DevCard::VictoryPoint,DevCard::VictoryPoint,
		DevCard::RoadBuilder,DevCard::RoadBuilder,
		DevCard::YearOfPlenty,DevCard::YearOfPlenty,
		DevCard::Monopoly,DevCard::Monopoly
};

void Map::GenerateHexes(int top_row_len, int mid_row_len) {
	assert(top_row_len >= 1);
	assert(mid_row_len >= top_row_len);
	std::vector<Resource> resources = MAP_RESOURCES;
	std::vector<int> dice_numbers = DICE_NUMBERS;
	std::shuffle(resources.begin(), resources.end(), gen);
	std::shuffle(dice_numbers.begin(), dice_numbers.end(), gen);

	int cur_row_len = top_row_len;
	int row_offset = 0;
	std::vector<Resource>::const_iterator rsc = resources.begin();
	std::vector<int>::const_iterator dice_num = dice_numbers.begin();
	for (int row = 0; cur_row_len >= top_row_len; ++row) {
		std::vector<std::optional<Hex>> new_row;
		for (int i = 0; i < row_offset; ++i) {
			new_row.push_back(std::nullopt);
		}
		for (int col = 0; col < cur_row_len; ++col) {
			if (*rsc == Resource::Desert) {
				new_row.push_back(Hex(std::pair<int, int>(row, col), 7, *rsc));
				robber_pos = { row, col };
			} else {

				new_row.push_back(Hex(std::pair<int, int>(row, col), *dice_num, *rsc));
				++dice_num;
			}
			++rsc;
		}
		hex_grid.push_back(new_row);
		if (row < mid_row_len - top_row_len) {
			cur_row_len++;
		} else {
			cur_row_len--;
			row_offset++;
		}
	}
}

void Map::GenerateNodes(int top_row_len, int mid_row_len) {
	assert(top_row_len >= 1);
	assert(mid_row_len >= top_row_len);
	assert(hex_grid.size() > 0);
	int row = 0;
	for (int cur_row_len = top_row_len; cur_row_len <= mid_row_len; ++cur_row_len) {
		std::vector<std::optional<Node>> new_row;
		for (int col = 0; col < cur_row_len * 2 + 1; ++col) {
			Node new_node(std::pair<int, int>(row, col));
			new_node.adj_hexes = Map::GetNodeHexes(std::pair<int, int>(row, col)); 
			new_row.push_back(new_node);
		}
		node_grid.push_back(new_row);
		row++;
	}
	for (int cur_row_len = mid_row_len; cur_row_len >= top_row_len; --cur_row_len) {
		std::vector<std::optional<Node>> new_row;
		int col = 0;
		for (; col < (mid_row_len - cur_row_len) * 2 + 1; ++col) {
			new_row.push_back(std::nullopt);
		}
		for (; col < mid_row_len * 2 + 2; ++col) {
			Node new_node(std::pair<int, int>(row, col));
			new_node.adj_hexes = Map::GetNodeHexes(std::pair<int, int>(row, col));
			new_row.push_back(new_node);
		}
		node_grid.push_back(new_row);
		row++;
	}
}

void Map::Generate(int top_row_len, int mid_row_len) {
	Map::GenerateHexes(top_row_len, mid_row_len);
	Map::GenerateNodes(top_row_len, mid_row_len);
	Map::GeneratePorts();
}

std::vector<Hex*> Map::GetNodeHexes(std::pair<int, int> node_pos) {
	int row = node_pos.first;
	int col = node_pos.second;
	std::vector<Hex*> hexes;
	if (row >= 0 && col >= 0) {
		if (col % 2 == 0) {
			if (row < hex_grid.size()) {
				if (col / 2 - 1 >= 0 && col / 2 - 1 < hex_grid[row].size() && hex_grid[row][col / 2 - 1]) {
					hexes.push_back(&(*hex_grid[row][col / 2 - 1]));
				}
				if (col / 2 >= 0 && col / 2 < hex_grid[row].size() && hex_grid[row][col / 2]) {
					hexes.push_back(&(*hex_grid[row][col / 2]));
				}
			}
			if (row - 1 >= 0 && row - 1 < hex_grid.size() && col / 2 - 1 >= 0 && col / 2 - 1 < hex_grid[row - 1].size() && hex_grid[row - 1][col / 2 - 1]) {

				hexes.push_back(&(*hex_grid[row - 1][col / 2 - 1]));
			}
		}
		else {
			if (row - 1 >= 0 && row - 1 < hex_grid.size()) {
				if (col / 2 - 1 >= 0 && col / 2 - 1 < hex_grid[row - 1].size() && hex_grid[row - 1][col / 2 - 1]) {
					hexes.push_back(&(*hex_grid[row - 1][col / 2 - 1]));
				}
				if (col / 2 >= 0 && col / 2 < hex_grid[row - 1].size() && hex_grid[row - 1][col / 2]) {
					hexes.push_back(&(*hex_grid[row - 1][col / 2]));
				}
			}
			if (row < hex_grid.size() && col / 2 >= 0 && col / 2 < hex_grid[row].size() && hex_grid[row][col / 2]) {
				hexes.push_back(&(*hex_grid[row][col / 2]));
			}
		}
	}
	return hexes;
}

std::vector<Node*> Map::GetHexNodes(std::pair<int, int> hex_pos) {
	int row = hex_pos.first;
	int col = hex_pos.second;
	std::vector<Node*> nodes;
	if (row >= 0 && row < hex_grid.size() && col >= 0 && col < hex_grid[row].size()) {
		nodes.push_back(&(*node_grid[row][col * 2]));
		nodes.push_back(&(*node_grid[row][col * 2 + 1]));
		nodes.push_back(&(*node_grid[row][col * 2 + 2]));
		nodes.push_back(&(*node_grid[row + 1][col * 2 + 1]));
		nodes.push_back(&(*node_grid[row + 1][col * 2 + 2]));
		nodes.push_back(&(*node_grid[row + 1][col * 2 + 3]));
	}
	return nodes;
}

std::vector<Node*> Map::GetAdjNodes(std::pair<int, int> node_pos) {
	int row = node_pos.first;
	int col = node_pos.second;
	std::vector<Node*> adj_nodes;
	if (col % 2 == 0) {
		if (row + 1 >= 0 && row + 1 < node_grid.size() 
			&& col + 1 >= 0 && col + 1 < node_grid[row + 1].size() 
			&& node_grid[row + 1][col + 1]) {
			adj_nodes.push_back(&node_grid[row + 1][col + 1].value());
		}
	}
	else {
		if (row - 1 >= 0 && row - 1 < node_grid.size()
			&& col - 1 >= 0 && col - 1 < node_grid[row - 1].size()
			&& node_grid[row - 1][col - 1]) {
			adj_nodes.push_back(&node_grid[row - 1][col - 1].value());
		}
	}
	if (row >= 0 && row < node_grid.size()
		&& col - 1 >= 0 && col - 1 < node_grid[row].size()
		&& node_grid[row][col - 1]) {
		adj_nodes.push_back(&node_grid[row][col - 1].value());
	}
	if (row >= 0 && row < node_grid.size()
		&& col + 1 >= 0 && col + 1 < node_grid[row].size()
		&& node_grid[row][col + 1]) {
		adj_nodes.push_back(&node_grid[row][col + 1].value());
	}
	return adj_nodes;
}


void Map::PrintHexes() {
	int offset = 12;
	for (std::vector<std::optional<Hex>>& row : hex_grid) {
		if (offset > 0) {
			std::cout << std::string(offset, ' ');
			offset -= 6;
		}
		for (std::optional<Hex>& opt : row) {
			if (opt) {
				std::string rsc_string = RscToString(opt->resource);
				rsc_string += std::string(6 - rsc_string.size(), ' ');
				std::string num_str = std::to_string(opt->dice_num);
				num_str += std::string(3 - num_str.size(), ' ');
				std::cout << '[' << num_str << ' ' << rsc_string << ']';
			}
			else {
				std::cout << std::string(6, ' ');
			}
		}
		std::cout << '\n';
	}
}

void Map::PrintNodes() {
	for (std::vector<std::optional<Node>>& row : node_grid) {
		for (std::optional<Node>& opt : row) {
			if (opt) {
				std::cout << "<> ";
			}
			else {
				std::cout << "   ";
			}
		}
		std::cout << '\n';
	}
}

std::vector<std::pair<Resource, int>> Map::GetResources(Occupation& occ) {
	std::vector<std::pair<Resource, int>> resources;
	for (Hex* hex : (*occ.node).adj_hexes) {
		if (hex->pos != robber_pos && hex->resource != Resource::Desert) {
			resources.push_back(std::pair<Resource, int>(hex->resource, hex->dice_num));
		}
	}
	return resources;
}

std::optional<std::vector<Player*>> Map::PlaceRobber(std::pair<int, int> pos) {
	if (robber_pos != pos && pos.first >= 0 && pos.first < hex_grid.size() 
		&& pos.second >= 0 && pos.second << hex_grid[pos.first].size() && hex_grid[pos.first][pos.second]) {
		robber_pos = pos;
		std::vector<Player*> adj_players;
		std::vector<Node*> adj_nodes = GetHexNodes(pos);
		for (Node*& node : adj_nodes) {
			if (node->occ) {
				adj_players.push_back(node->occ->player);
			}
		}
		return adj_players;
	}
	return std::nullopt;
}

bool Map::PlaceOcc(std::shared_ptr<Occupation> occ, bool needs_road) {
	Node& node = node_grid[(*occ->node).pos.first][(*occ->node).pos.second].value();
	if (node.occ) {
		return false;
	}
	std::vector<Node*> adj_nodes = GetAdjNodes(node.pos);
	for (Node* adj_node : adj_nodes) {
		if (adj_node->occ) {
			return false;
		}
	}
	if (needs_road) {
		bool has_road = false;
		if (roads.find(&node) == roads.end()) {
			return false;
		}
		for (std::pair<Node*, Player*>& road : roads.at(&node)) {
			if (*(road.second) == *occ->player) {
				has_road = true;
				break;
			}
		}
		if (!has_road) {
			return false;
		}
	}
	node.occ = occ;
	occ->player->occupations.push_back(occ);
	if (port_map.find(node.pos) != port_map.end()) {
		std::vector<std::pair<Resource, int>>& discounts = port_map[node.pos];
		for (std::pair<Resource, int>& discount : discounts) {
			occ->player->discounts[discount.first] = discount.second;
		}
	}
	return true;
}

bool Map::ReplaceOcc(std::shared_ptr<Occupation> occ) {
	Node& node = node_grid[(*occ->node).pos.first][(*occ->node).pos.second].value();
	if (!node.occ || *node.occ->player != *occ->player) {
		// Node does not have occupation to replace, or current occupation owner is different.
		return false;
	}
	node.occ = occ;
	return true;
}

void Map::GeneratePorts() {
	std::vector<std::vector<std::pair<Resource, int>>> discounts = PORT_DISCOUNTS;
	std::shuffle(discounts.begin(), discounts.end(), gen);
	std::vector<std::vector<std::pair<Resource, int>>>::iterator discount = discounts.begin();
	for (const std::pair<std::pair<int, int>, std::pair<int, int>>& port_location : PORT_LOCATIONS) {
		port_map[port_location.first] = *discount;
		port_map[port_location.second] = *discount;
		++discount;
	}
}

void Map::ShuffleDevcards() {
	std::vector<DevCard> devcards = DEVCARDS;
	std::shuffle(devcards.begin(), devcards.end(), gen);
	for (DevCard& devcard : devcards) {
		devcard_deck.push(devcard);
	}
}

std::optional<DevCard> Map::DrawDevcard() {
	if (!devcard_deck.empty()) {
		DevCard devcard = devcard_deck.top();
		devcard_deck.pop();
		return devcard;
	}
	return std::nullopt;
}

std::vector<Node*> Map::GetPlayerNodes(Player& player) {
	std::unordered_set<Node*> player_nodes;
	for (std::shared_ptr<Occupation>& occ : player.occupations) {
		std::stack<Node*> to_visit;
		std::unordered_set<Node*> visited;
		to_visit.push(occ->node);
		while (!to_visit.empty()) {
			Node* cur = to_visit.top();
			to_visit.pop();
			visited.insert(cur);
			player_nodes.insert(cur);
			std::vector<std::pair<Node*, Player*>>& node_roads = roads[cur];
			for (std::pair<Node*, Player*>& road : node_roads) {
				if (*road.second == player && visited.find(road.first) == visited.end()) {
					to_visit.push(road.first);
				}
			}
		}
	}
	return std::vector<Node*>(player_nodes.begin(), player_nodes.end());
}

int Map::GetLongestRoad(Player& player) {
	std::vector<Node*> plr_nodes = GetPlayerNodes(player);
	int longest_road = 0;
	for (Node*& root : plr_nodes) {
		std::stack<std::pair<Node*, int>> to_visit; // { Node to visit, Depth }
		std::unordered_set<Node*> visited;
		to_visit.push({ root, 0 });

		while (!to_visit.empty()) {
			std::pair<Node*, int> cur = to_visit.top();
			to_visit.pop();
			visited.insert(cur.first);
			if (cur.second > longest_road) {
				longest_road = cur.second;
			}
			std::vector<std::pair<Node*, Player*>>& node_roads = roads[cur.first];
			for (std::pair<Node*, Player*>& road : node_roads) {
				if (*road.second == player && visited.find(road.first) == visited.end()) {
					to_visit.push({ road.first, cur.second + 1 });
				}
			}
		}
	}
	return longest_road;
}

bool Map::PlaceRoad(Player& player, std::pair<int, int> node_pos1, std::pair<int, int> node_pos2) {
	Node* node1 = &node_grid[node_pos1.first][node_pos1.second].value();
	Node* node2 = &node_grid[node_pos2.first][node_pos2.second].value();
	std::vector<std::pair<Node*, Player*>> node1_roads = roads[node1];
	std::vector<std::pair<Node*, Player*>> node2_roads = roads[node2];

	std::vector<Node*> node1_adj = GetAdjNodes(node_pos1);
	bool nodes_adjacent = false;
	for (Node*& node : node1_adj) {
		if (node == node2) {
			// Nodes are adjacent
			nodes_adjacent = true;
			break;
		}
	}
	if (!nodes_adjacent) {
		// Nodes are not adjacent, cannot place road
		return false;
	}

	bool valid_road = false;
	for (std::pair<Node*, Player*>& road : node1_roads) {
		if (road.first == node2) {
			// Road already exists
			return false;
		}
		if (*road.second == player) {
			// Player has road connecting to node1
			valid_road = true;
		}
	}
	if (!valid_road) {
		if (node1->occ && *node1->occ->player == player) {
			// Player has occupation on node1
			valid_road = true;
		}
		else if (node2->occ && *node2->occ->player == player) {
			// Player has occupation on node2
			valid_road = true;
		}
		else {
			for (std::pair<Node*, Player*>& road : node2_roads) {
				if (*road.second == player) {
					// Player has road connecting to node2
					valid_road = true;
				}
			}
		}
	}
	if (!valid_road) {
		// Road placement invalid
		return false;
	}
	// Road placement valid
	// Place the road
	roads[node1].push_back({ node2, &player });
	roads[node2].push_back({ node1, &player });
	player.longest_road = GetLongestRoad(player);
	return true;
}

std::optional<std::pair<int, int>> Map::OccGetRandPos(Player& player, bool needs_road) {
	for (std::vector<std::optional<Node>>& row : node_grid) {
		for (std::optional<Node>& node_opt : row) {
			if (node_opt) {
				Node& node = node_opt.value();
				if (!node.occ) {
					bool adj_occ = false;
					std::vector<Node*> adj_nodes = GetAdjNodes({ node.pos.first, node.pos.second });
					for (Node*& adj_node_ptr : adj_nodes) {
						if (adj_node_ptr->occ) {
							adj_occ = true;
							break;
						}
					}
					if (adj_occ) {
						// Adjacent occupation to node, cannot place here
						continue;
					}
					if (needs_road) {
						bool has_road = false;
						for (std::pair<Node*, Player*>& road : roads[&node]) {
							if (*road.second == player) {
								has_road = true;
								break;
							}
						}
						if (!has_road) {
							// No valid adjacent roads, cannot place here
							continue;
						}
					}
					// No adjacent occupations and, if needed, a valid road exists. Can place here!
					return std::make_optional<std::pair<int, int>>( node.pos.first, node.pos.second );
				}
			}
		}
	}
	// All nodes invalid, return nullopt
	return std::nullopt;
}

bool Map::RoadExists(std::pair<int, int> node_pos1, std::pair<int, int> node_pos2) {
	if (node_pos1.first < node_grid.size() && node_pos1.second < node_grid[node_pos1.first].size() && node_grid[node_pos1.first][node_pos1.second] &&
		node_pos2.first < node_grid.size() && node_pos2.second < node_grid[node_pos2.first].size() && node_grid[node_pos2.first][node_pos2.second]) {
		// Nodes exist, do rest
		Node& node1 = node_grid[node_pos1.first][node_pos1.second].value();
		Node& node2 = node_grid[node_pos2.first][node_pos2.second].value();
		for (std::pair<Node*, Player*>& road : roads[&node1]) {
			if (road.first == &node2) {
				return true;
			}
		}
		return false;
	}
}

std::optional<std::pair<std::pair<int, int>, std::pair<int, int>>> Map::RoadGetRandPos(Player& player, std::optional<std::shared_ptr<Occupation>> occ) {
	if (occ != std::nullopt && occ.value()) {
		std::vector<Node*> adj_nodes = GetAdjNodes(occ.value()->node->pos);
		for (Node*& adj_node_ptr : adj_nodes) {
			if (!RoadExists(occ.value()->node->pos, adj_node_ptr->pos)) {
				return { { occ.value()->node->pos, adj_node_ptr->pos } };
			}
		}
		return std::nullopt;
	}
	else {
		std::vector<Node*> plr_nodes = GetPlayerNodes(player);
		for (Node*& node : plr_nodes) {
			std::vector<Node*> adj_nodes = GetAdjNodes(node->pos);
			for (Node*& adj_node_ptr : adj_nodes) {
				if (!RoadExists(node->pos, adj_node_ptr->pos)) {
					return { { node->pos, adj_node_ptr->pos } };
				}
			}
		}
		return std::nullopt;
	}
}