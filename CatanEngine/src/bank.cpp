#include "bank.h"

// See bank.h for documentation
bool Bank::Process(Request& request) {
	if (inventory[request.resource] >= request.amount) {
		inventory[request.resource] -= request.amount;
		request.player->resources[request.resource] += request.amount;
		return true;
	}
	else {
		return false;
	}
}

// See bank.h for documentation
bool Bank::ProcessMultiple(std::vector<Request>& requests) {
	std::unordered_map<Resource, int> inv_copy = inventory;
	for (Request& request : requests) {
		inv_copy[request.resource] -= request.amount;
		if (inv_copy[request.resource] < 0) { return false; }
	}
	for (Request& request : requests) {
		request.player->resources[request.resource] += request.amount;
	}
	inventory = inv_copy;
	return true;
}

Bank::Bank() {
	inventory[Resource::Wool] = 19;
	inventory[Resource::Grain] = 19;
	inventory[Resource::Lumber] = 19;
	inventory[Resource::Brick] = 19;
	inventory[Resource::Ore] = 19;
}

Bank::Bank(int starting_amount) {
	inventory[Resource::Wool] = starting_amount;
	inventory[Resource::Grain] = starting_amount;
	inventory[Resource::Lumber] = starting_amount;
	inventory[Resource::Brick] = starting_amount;
	inventory[Resource::Ore] = starting_amount;
}

Bank::operator std::string() const{
	return
		std::to_string(inventory.at(Resource::Wool)) + " Wool, " + 
		std::to_string(inventory.at(Resource::Grain)) + " Grain, " +
		std::to_string(inventory.at(Resource::Lumber)) + " Lumber, " +
		std::to_string(inventory.at(Resource::Brick)) + " Brick, " +
		std::to_string(inventory.at(Resource::Ore)) + " Ore, ";
}