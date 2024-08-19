#pragma once

#include <vector>
#include <unordered_map>
#include "resource.h"

enum class Decision {
	OPT_PreDice, // Pick an action before rolling dice (Either play dev card or roll the dice)
				 // 0: roll dice, 1: play dev (if dev cards in inventory)
	OPT_Turn, // Pick an action to do on your turn
			  // 0: pass, 1: trade, 2: play dev, 3: draw dev, 4: road, 5: settle, 6: city
	POS_BuildRoad, // Pick a location to place road
	POS_Settle, // Pick a location to settle
	POS_City, // Pick a location to upgrade to city
	OPT_PlayDev, // Pick a dev card (from inventory) to play
	             // 0: knight, 1: YOP, 2: road builder, 3: monopoly
	TRD_Trade, // Pick a trade 
	OPT_TradeResponses, // Pick, from all available trades, to take one or not to take any (including counteroffers)
	POS_Robber, // Pick which hex to place the robber
	OPT_Steal, // Pick which player (out of those available) to steal a resource from after robber the adj. hex
	           // 0, 1, 2
	RSC_Monopoly, // Pick which resource to steal from all players
		          // game will only consider first resource in vector
	RSC_YOP, // Pick two resources to take from bank
	         // game will only consider first two resources in vector
	POS_RoadBuilder, // Pick a position to place road, given that you can place another one immediately after 
	RSC_Discard, // Pick resources to discard, after 7 has rolled and you have too many cards
	POS_IniSettle, // Pick initial settlement location
	POS_IniRoad, // Pick road location next to most recent initial settlement
	OPT_TradeRespond, // Pick a response to another player's trade
	                  // 0: decline, 1: accept, 2: counter offer
	TRD_TradeCounter, // Pick a counteroffer trade to another player's trade
	OPT_TradeRespondCounter, // Pick a response to another player's counteroffer to another player's trade
	                         // 0: decline, 1: accept, 2: counter offer
};

enum class DecisionType {
	Option, Position, Trade, Resource
};

const std::unordered_map<Decision, std::string> decision_map_str = {
	{Decision::OPT_PreDice, "Pick Pre-Diceroll action\n0: Roll Dice, 1: Play a dev card"},
	{Decision::OPT_Turn, "Pick a turn action\n0: Pass, 1: Trade, 2: Play a dev card, 3: Draw a dev card, 4: Build a road, 5: Settle, 6: City"},
	{Decision::POS_BuildRoad, "Pick a road location\nFormat: 'row col row col' e.g. '2 11 2 12' -> Road from node at 2,11 to node at 2,12"},
	{Decision::POS_Settle, "Pick a settlement location\nFormat: 'row col' e.g. '1 0'"},
	{Decision::POS_City, "Pick a city location\nFormat: 'row col' e.g. '1 0'"},
	{Decision::OPT_PlayDev, "Pick a dev card to play from inventory\n0: Knight, 1: Year of Plenty, 2: Road Builder, 3: Monopoly"},
	{Decision::TRD_Trade, "Pick a trade to offer to other players\nFormat: 'LBOGW|LBOGW' e.g. '20000|02000' -> Request 2 lumber, Give 2 brick"},
	{Decision::OPT_TradeResponses, "Pick a trade to accept, or to accept none\n0: Accept none, [Trade #]: Accept corresponding trade"},
	{Decision::POS_Robber, "Pick a robber location\nFormat: 'row col' e.g. '1 0'"},
	{Decision::OPT_Steal, "Pick a player to steal from\nFormat: [Option #]: Steal from corresponding player, e.g. '1' -> steal from Option 1"},
	{Decision::RSC_Monopoly, "Pick a resource to steal from everyone\nL: Lumber, B: Brick, O: Ore, G: Grain, W: Wool"},
	{Decision::RSC_YOP, "Pick two resources to take from bank\nFormat: '[Rsc1][Rsc2]' e.g. 'BO' -> Brick & Ore, 'LL' -> Lumber & Lumber"},
	{Decision::RSC_Discard, "Pick resources to discard\nFormat: '[Rsc1][Rsc2]...[RscN]' e.g. 'LLBW' -> Lumber x2, Brick, Wool"},
	{Decision::POS_IniSettle, "Pick a settlement location\nFormat: 'row col' e.g. '1 0'"},
	{Decision::POS_IniRoad, "Pick a road location\nFormat: 'row col row col' e.g. '2 11 2 12' -> Road from node at 2,11 to node at 2,12"},
	{Decision::OPT_TradeRespond, "Pick a response to the trade\n0: Decline, 1: Accept, 2: Counter Offer"},
	{Decision::TRD_TradeCounter, "Pick a counteroffer trade\nFormat: 'LBOGW|LBOGW' e.g. '20000|02000' -> Request 2 lumber, Give 2 brick"},
	{Decision::OPT_TradeRespondCounter, "Pick a response to the counter offer\n0: Decline, 1: Accept, 2: Counter offer"}
};

const std::unordered_map<Decision, DecisionType> decision_map_type = {
	{Decision::OPT_PreDice, DecisionType::Option},
	{Decision::OPT_Turn, DecisionType::Option},
	{Decision::POS_BuildRoad, DecisionType::Position},
	{Decision::POS_Settle, DecisionType::Position},
	{Decision::POS_City, DecisionType::Position},
	{Decision::OPT_PlayDev, DecisionType::Option},
	{Decision::TRD_Trade, DecisionType::Trade},
	{Decision::OPT_TradeResponses, DecisionType::Option},
	{Decision::POS_Robber, DecisionType::Position},
	{Decision::OPT_Steal, DecisionType::Option},
	{Decision::RSC_Monopoly, DecisionType::Resource},
	{Decision::RSC_YOP, DecisionType::Resource},
	{Decision::RSC_Discard, DecisionType::Resource},
	{Decision::POS_IniSettle, DecisionType::Position},
	{Decision::POS_IniRoad, DecisionType::Position},
	{Decision::OPT_TradeRespond, DecisionType::Option},
	{Decision::TRD_TradeCounter, DecisionType::Trade},
	{Decision::OPT_TradeRespondCounter, DecisionType::Option}
};

class DecisionResult {
public:
	virtual ~DecisionResult() = default;
};

class PositionDecisionResult : public DecisionResult {
public:
	const std::vector<std::pair<int, int>> positions;

	PositionDecisionResult(const std::vector<std::pair<int, int>> positions) : positions(positions) {}
};

class ResourceDecisionResult : public DecisionResult {
public:
	const std::vector<Resource> resources;

	ResourceDecisionResult(const std::vector<Resource> resources) : resources(resources) {}
};

class OptionDecisionResult : public DecisionResult {
public:
	const int option;

	OptionDecisionResult(const int option) : option(option) {}
};

class TradeDecisionResult : public DecisionResult {
public:
	const std::unordered_map<Resource, int> offer;
	const std::unordered_map<Resource, int> request;

	TradeDecisionResult(const std::unordered_map<Resource, int> offer, const std::unordered_map<Resource, int> request) :
		offer(offer), request(request) {}
};