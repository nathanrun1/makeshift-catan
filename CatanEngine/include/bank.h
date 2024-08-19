#pragma once

#include <unordered_map>
#include <cassert>
#include "resource.h"
#include "player.h"




class Bank {
public:
	class Request {
	public:
		const Resource resource;
		const unsigned int amount;
		Player* const player;

		Request(Player* const player, const Resource resource, const unsigned int amount) : player(player),
			resource(resource), amount(amount) {}
	};

	std::unordered_map<Resource, int> inventory;

	/// <summary>
	/// Determines if bank holds enough resources to fulfill request. If so, subtracts requested resources
	/// from bank and adds them to request's player's inventory. If not, the request is not fulfilled and
	/// bank remains unchanged.
	/// </summary>
	/// <returns>True if request fulfilled, false otherwise</returns>
	bool Process(Request& request);

	/// <summary>
	/// Determines if bank holds enough resources to fulfill requests. Checks for eaach resource.
	/// If enough of a resource to fulfill all requests of that resource, fulfills those.
	/// If ever not enough of a resource to fulfill all requests of that resource, will not fulfill any
	/// requests of that resource UNLESS only one player is requesting it.
	/// </summary>
	/// <returns>True if all requests fulfilled, false otherwise</returns>
	bool ProcessMultiple(std::vector<Request>& requests);


	Bank();

	Bank(int starting_amount);
	
	operator std::string() const;
};