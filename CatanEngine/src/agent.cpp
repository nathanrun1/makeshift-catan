#include "agent.h"

std::unique_ptr<DecisionResult> Agent::Decide(GameState& game_state, Decision decision) {
	if (agent_type == AgentType::IO) {
		Catan_IO::Prompt(decision_map_str.at(decision));
		switch (decision_map_type.at(decision)) {
			case DecisionType::Option: {
				int option;
				std::cin >> option;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				return std::make_unique<OptionDecisionResult>(option);
			}
			case DecisionType::Position: {
				std::vector<std::pair<int, int>> positions;
				std::string input;
				Catan_IO::GetLine(input);

				std::stringstream ss(input);
				int row, col;
				while (ss >> row >> col) {
					positions.emplace_back(row, col);
				}
				return std::make_unique<PositionDecisionResult>(positions);
			}
			case DecisionType::Resource: {
				std::vector<Resource> resources;
				std::string input;
				Catan_IO::GetLine(input);
				for (char& c : input) {
					switch (c) {
					case 'L':
						resources.push_back(Resource::Lumber);
						break;
					case 'B':
						resources.push_back(Resource::Brick);
						break;
					case 'O':
						resources.push_back(Resource::Ore);
						break;
					case 'G':
						resources.push_back(Resource::Grain);
						break;
					case 'W':
						resources.push_back(Resource::Wool);
						break;
					default:
						resources.push_back(Resource::Lumber);
						break;
					}
				
				}
				return std::make_unique<ResourceDecisionResult>(resources);
			}
			case DecisionType::Trade: {
				std::unordered_map<Resource, int> offer;
				std::unordered_map<Resource, int> request;
				std::string input;
				Catan_IO::GetLine(input);
				if (input.size() != 11 || input[5] != '|') { return nullptr; }
				request[Resource::Lumber] = std::clamp(input[0] - '0', 0, 3);
				request[Resource::Brick] = std::clamp(input[1] - '0', 0, 3);
				request[Resource::Ore] = std::clamp(input[2] - '0', 0, 3);
				request[Resource::Grain] = std::clamp(input[3] - '0', 0, 3);
				request[Resource::Wool] = std::clamp(input[4] - '0', 0, 3);
				offer[Resource::Lumber] = std::clamp(input[6] - '0', 0, 3);
				offer[Resource::Brick] = std::clamp(input[7] - '0', 0, 3);
				offer[Resource::Ore] = std::clamp(input[8] - '0', 0, 3);
				offer[Resource::Grain] = std::clamp(input[9] - '0', 0, 3);
				offer[Resource::Wool] = std::clamp(input[10] - '0', 0, 3);
				return std::make_unique<TradeDecisionResult>(offer, request);
			}
		}
	}
	else {
		return nullptr; // temp
	}
}