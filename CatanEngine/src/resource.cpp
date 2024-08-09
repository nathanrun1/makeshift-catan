#include "resource.h"
#include <string>

const std::vector<Resource> RESOURCES = { Resource::Wool, Resource::Grain, Resource::Lumber, Resource::Brick, Resource::Ore };

std::string RscToString(Resource rsc) {
	switch (rsc) {
		case Resource::Wool: return "Wool";
		case Resource::Grain: return "Grain";
		case Resource::Lumber: return "Lumber";
		case Resource::Brick: return "Brick";
		case Resource::Ore: return "Ore";
		case Resource::Desert: return "Desert";
	default: return "UNKNOWN";
	}
}