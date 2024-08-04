#include <iostream>
#include <vector>
#include <utility>
#include <optional>
#include <assert.h>
#include <random>
#include <algorithm>
#include "resource.h"
#include "map.h"


const std::vector<Resource> RESOURCES = {
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

// See map.h for documentation
void Map::GenerateHexes(int top_row_len, int mid_row_len) {
	assert(top_row_len >= 1);
	assert(mid_row_len >= top_row_len);
	std::vector<Resource> resources = RESOURCES;
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
				new_row.push_back(Hex(7, *rsc));
			} else {
				new_row.push_back(Hex(*dice_num, *rsc));
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

// See map.h for documentation
void Map::GenerateNodes(int top_row_len, int mid_row_len) {
	assert(top_row_len >= 1);
	assert(mid_row_len >= top_row_len);
	for (int cur_row_len = top_row_len; cur_row_len <= mid_row_len; ++cur_row_len) {
		std::vector<std::optional<Node>> new_row;
		for (int n = 0; n < cur_row_len * 2 + 1; ++n) {
			new_row.push_back(Node());
		}
		node_grid.push_back(new_row);
	}
	for (int cur_row_len = mid_row_len; cur_row_len >= top_row_len; --cur_row_len) {
		std::vector<std::optional<Node>> new_row;
		for (int i = 0; i < (mid_row_len - cur_row_len) * 2 + 1; ++i) {
			new_row.push_back(std::nullopt);
		}
		for (int n = 0; n < cur_row_len * 2 + 1; ++n) {
			new_row.push_back(Node());
		}
		node_grid.push_back(new_row);
	}
}

void Map::Generate(int top_row_len, int mid_row_len) {
	Map::GenerateHexes(top_row_len, mid_row_len);
	Map::GenerateNodes(top_row_len, mid_row_len);
}

// See map.h for documentation
std::vector<Hex*> Map::GetNodeHexes(int row, int col) {
	assert(row >= 0 && row < node_grid.size());
	assert(col >= 0 && col < node_grid[row].size());
	std::vector<Hex*> hexes;
	if (col % 2 == 0) {
		if (row < hex_grid.size()) {
			if (col / 2 - 1 >= 0 && col / 2 - 1 < hex_grid[row].size()) {
				hexes.push_back(&(*hex_grid[row][col / 2 - 1]));
			}
			if (col / 2 >= 0 && col / 2 < hex_grid[row].size()) {
				hexes.push_back(&(*hex_grid[row][col / 2]));
			}
		}
		if (row - 1 >= 0 && row - 1 < hex_grid.size() && col / 2 - 1 >= 0 && col / 2 - 1 < hex_grid[row - 1].size()) {

			hexes.push_back(&(*hex_grid[row - 1][col / 2 - 1]));
		}
	}
	else {
		if (row - 1 >= 0 && row - 1 < hex_grid.size()) {
			if (col / 2 - 1 >= 0 && col / 2 - 1 < hex_grid[row - 1].size()) {
				hexes.push_back(&(*hex_grid[row - 1][col / 2 - 1]));
			}
			if (col / 2 >= 0 && col / 2 < hex_grid[row - 1].size()) {
				hexes.push_back(&(*hex_grid[row - 1][col / 2]));
			}
		}
		if (row < hex_grid.size() && col / 2 >= 0 && col / 2 < hex_grid[row].size()) {

			hexes.push_back(&(*hex_grid[row][col / 2]));
		}
	}
	return hexes;
}

// See map.h for documentation
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


// See map.h for documentation
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

// See map.h for documentation
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