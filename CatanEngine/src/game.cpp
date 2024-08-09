#include "game.h"

Game::Game(int vp_to_win) : vp_to_win(vp_to_win), m_gen(m_rd()) {}

int Game::DiceRoll() {
	std::uniform_int_distribution dist(1, 6);
	return dist(m_gen) + dist(m_gen);
}


void Game::GetPlayers() {
	for (int i = 0; i < 4; ++i) {
		players.push_back(Player("Player" + std::to_string(i + 1)));
	}
}

void Game::SetupPhase() {
	for (std::vector<Player>::iterator it = players.begin(); it != players.end(); ++it) {
		// Prompt settlement placement location from player (either thru input or sum else if its an RL model)
		// Request settlement placement at location from map
			// If not successful, re-request placement location. 
				// If same issue, place at random valid spot.
		// Prompt road placement location from player (must be adj. to recent settlement)
		// Check that road is adj. to first settlement, and request placement onto map
			// If not successful, re-request placement location.
				// If same issue, place at random valid spot.
	}
	for (std::vector<Player>::iterator it = players.end(); it != players.begin(); --it) {
		// same shit
	}
}

void Game::PlayerTurn(Player& player) {
	int dice_roll = DiceRoll();
	if (dice_roll != 7) {
		for (Player& player : players) {
			// for each occupation
				// Request available resources at occupation's node from map
				// If any match dice_roll:
					// Add amnt of that resource corresponding to occupation type
		}
	}
	else {
		// Prompt player to pick new robber location
			// Request robber placement on map
				// If unsuccessful, place on random spot
		// Get players adjacent to robber hex
		// Give player option from those players who have a non-empty inventory to steal 1 random resource
		// Transfer 1 random resource from robber victim to player
	}
	// From player, listen for: Play Dev Card, Build Settlement, Build Road, Upgrade to City, Draw Dev Card, Trade
	// Play Dev Card
		// Check if player has this dev card. If so, run the dev card's behavior
		// Player can only do this action once per turn.
	// Build Settlement
		// Check if player can afford. If so, send build request to map. Send feedback to player whether build was successful.
		// (Map here handles player discount adjustment, probably)
		// Adjust VPs and Resource inventory accordingly.
		// Check for win
	// Upgrade to City
		// Check if player can afford. If so, send upgrade request to map. Send feedback to player whether upgrade was successful.
		// Adjust VPs and Resource inventory accordingly.
		// Check for win
	// Build Road
		// Check if player can afford. If so, send road build request to map. Send feedback to player whether build was successful.
		// Update player's longest road length
		// Check if player longest road is now bigger than current, if so assign VPs accordingly & check for win.
	// Draw Dev Card
		// Check if player can afford. If so, add random dev card from top of deck to player dev card inventory.
		// If VP, then adjust player VPs & check for win, card won't be playable.
		// Adjust Resource inventory accordingly.
	// Trade
		// Prompt player to pick resources to give and resources to take (both sides can't share resource types)
		// Prompt all other players to either: Accept trade, Decline trade, Send counteroffer
		// Prompt player with all other player responses. Player can either: Confirm a trade, Accept a counteroffer (which confirms it), or do nothing.
	// Player can do any of these actions an indefinite amount of times unless indicated otherwise (ideally under a time limit)
}

void Game::Start() {
	while (true) {
		for (Player& player : players) {
			PlayerTurn(player);
			if (player.victory_points >= vp_to_win) {
				// Double check (should be checked within PlayerTurn probably)
				Win(player);
				return;
			}
		}
		std::cin.get(); // temp: prevent inf loop
	}
}

void Game::Win(Player& player) {
	std::cout << player.name << " has won Catan!" << std::endl;
}