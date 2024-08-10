#include <iostream>
#include <random>
#include <algorithm>
#include "map.h"

Map::Map() {
	Generate(3, 5);
}

Map::Map(int top_row_len, int mid_row_len) {
	Generate(top_row_len, mid_row_len);
}

const std::vector<Resource> MAP_RESOURCES = {
	Resource::Ore, Resource::Ore, Resource::Ore,
	Resource::Wool, Resource::Wool, Resource::Wool, Resource::Wool,
	Resource::Grain, Resource::Grain, Resource::Grain, Resource::Grain,
	Resource::Brick, Resource::Brick, Resource::Brick,
	Resource::Lumber, Resource::Lumber, Resource::Lumber, Resource::Lumber,
	Resource::Desert
};

const std::vector<int> DICE_NUMBERS = {
	2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12
};

void Map::GenerateHexes(int top_row_len, int mid_row_len) {
	assert(top_row_len >= 1);
	assert(mid_row_len >= top_row_len);
	std::vector<Resource> resources = MAP_RESOURCES;
	std::vector<int> dice_numbers = DICE_NUMBERS;
	std::random_device rd;
	std::mt19937 g1(rd());
	std::mt19937 g2(rd());
	std::shuffle(resources.begin(), resources.end(), g1);
	std::shuffle(dice_numbers.begin(), dice_numbers.end(), g2);

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
			new_node.adj_hexes = Map::GetNodeHexes(row, col); // error is here
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
			// std::cout << "[INFO]: Calling GetNodeHexes(" << row << ", " << col << ");" << '\n';
			new_node.adj_hexes = Map::GetNodeHexes(row, col);
			new_row.push_back(new_node);
		}
		node_grid.push_back(new_row);
		row++;
	}
}

void Map::Generate(int top_row_len, int mid_row_len) {
	Map::GenerateHexes(top_row_len, mid_row_len);
	Map::GenerateNodes(top_row_len, mid_row_len);
}

std::vector<Hex*> Map::GetNodeHexes(int row, int col) {
	assert(row >= 0);
	assert(col >= 0);
	std::vector<Hex*> hexes;
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
	return hexes;
}

std::vector<Node*> Map::GetHexNodes(int row, int col) {
	assert(row >= 0 && row < hex_grid.size());
	assert(col >= 0 && col < hex_grid[row].size());
	std::vector<Node*> nodes;
	nodes.push_back(&(*node_grid[row][col * 2]));
	nodes.push_back(&(*node_grid[row][col * 2 + 1]));
	nodes.push_back(&(*node_grid[row][col * 2 + 2]));
	nodes.push_back(&(*node_grid[row + 1][col * 2 + 1]));
	nodes.push_back(&(*node_grid[row + 1][col * 2 + 2]));
	nodes.push_back(&(*node_grid[row + 1][col * 2 + 3]));
	return nodes;
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
	std::vector<Hex*> adj_hexes = GetNodeHexes(occ.node.pos.first, occ.node.pos.second);
	std::vector<std::pair<Resource, int>> resources;
	for (Hex* hex : adj_hexes) {
		if (hex->pos != robber_pos) {
			resources.push_back(std::pair<Resource, int>(hex->resource, hex->dice_num));
		}
	}
	return resources;
}