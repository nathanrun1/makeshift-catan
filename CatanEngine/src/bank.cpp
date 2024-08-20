#include "bank.h"

// See bank.h for documentation
bool Bank::Process(Request& request) {
	if (inventory[request.resource] >= request.amount) {
		inventory[request.resource] -= request.amount;
		request.player->resources[request.resource] += request.amount;
		Catan_IO::Info(request.player->name + " took " + std::to_string(request.amount) + " " + RscToString(request.resource) + " from bank.");
		return true;
	}
	else {
		return false;
	}
}

// See bank.h for documentation
bool Bank::ProcessMultiple(std::vector<Request>& requests) {
	bool all_fulfilled = true;
	std::unordered_map<Resource, std::unordered_map<Player*, int>> cumulative_requests;
	for (Request& request : requests) {
		cumulative_requests[request.resource][request.player] += request.amount;
	}
	for (std::pair<const Resource, std::unordered_map<Player*, int>>& cumulative_rsc_request : cumulative_requests) {
		int amnt_requested = 0;
		for (std::pair<Player* const, int>& cumulative_plr_request : cumulative_rsc_request.second) {
			amnt_requested += cumulative_plr_request.second;
		}
		if (amnt_requested <= inventory[cumulative_rsc_request.first]) {
			inventory[cumulative_rsc_request.first] -= amnt_requested;
			for (std::pair<Player* const, int>& cumulative_plr_request : cumulative_rsc_request.second) {
				cumulative_plr_request.first->resources[cumulative_rsc_request.first] += cumulative_plr_request.second;
				Catan_IO::Info(cumulative_plr_request.first->name + " took " + std::to_string(cumulative_plr_request.second) + " " + 
					RscToString(cumulative_rsc_request.first) + " from bank.");
			}
		}
		else {
			if (inventory[cumulative_rsc_request.first] > 0 && cumulative_rsc_request.second.size() == 1) {
				Player* player = cumulative_rsc_request.second.begin()->first;
				player->resources[cumulative_rsc_request.first] += inventory[cumulative_rsc_request.first];
				Catan_IO::Info(player->name + " took " + std::to_string(inventory[cumulative_rsc_request.first]) + " " +
					RscToString(cumulative_rsc_request.first) + " from bank.");
				inventory[cumulative_rsc_request.first] = 0;
			}
			else {
				Catan_IO::Info("Not enough " + RscToString(cumulative_rsc_request.first) + " in bank.");
			}
			all_fulfilled = false;
		}
	}
	return all_fulfilled;
}
// 
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