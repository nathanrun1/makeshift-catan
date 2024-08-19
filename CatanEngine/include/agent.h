#pragma once

#include <memory>
#include <iostream>
#include <algorithm>
#include <limits>
#include <sstream>
#include "decision.h"
#include "gamestate.h"
#include "catan_io.h"

enum class AgentType {
	IO, RL
};

class Agent {
public:
	AgentType agent_type;

	Agent(AgentType agent_type) : agent_type(agent_type) {}

	std::unique_ptr<DecisionResult> Decide(GameState& game_state, Decision decision);
};