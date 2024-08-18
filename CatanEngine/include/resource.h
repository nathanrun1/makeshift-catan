#pragma once

#include <string>
#include <vector>

enum class Resource {
	Wool, Grain, Lumber, Brick, Ore, Desert
};

extern std::string RscToString(Resource rsc);

extern const std::vector<Resource> RESOURCES;
