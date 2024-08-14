#pragma once

#include <vector>
#include <random>
#include <iostream>
#include "map.h"
#include "player.h"
#include "bank.h"
#include "buildings.h"

class Player;

class Game {
private:
	std::random_device m_rd;
	std::mt19937 m_gen;

	/// <summary>
	/// Initialize up to 4 players
	/// </summary>
	void GetPlayers();

	/// <summary>
	/// Run the Catan setup phase, where players place initial settlements and roads.
	/// </summary>
	void SetupPhase();

	/// <summary>
	/// Run a given player's turn
	/// </summary>
	/// <param name="player">- The player who currently has action</param>
	void PlayerTurn(Player& player);

	/// <summary>
	/// Declares player as winner and that the game is over.
	/// </summary>
	/// <param name="player">- Player that will be declared as winner</param>
	void Win(Player& player);
public:
	std::vector<Player> players;
	Map map;
	Bank bank;

	const int vp_to_win;

	/// <summary>
	/// Constructs a Game instance
	/// </summary>
	/// <param name="vp_to_win">- Amount of victory points to win the game. Default is 10.</param>
	Game(int vp_to_win = 10);

	/// <summary>
	/// Rolls a 2d6
	/// </summary>
	/// <returns>Result of a 2d6 roll</returns>
	int DiceRoll();

	/// <summary>
	/// Starts the Catan game. Will continue to run until there is a winner.
	/// </summary>
	void Start();
};

