#pragma once

#include <string>

enum class Resource {
	Wool, Grain, Lumber, Brick, Ore, Desert
};

extern std::string RscToString(Resource rsc);

