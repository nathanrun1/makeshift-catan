#pragma once

#include <unordered_map>
#include <cassert>
#include "resource.h"


class Request {
public:
	const Resource resource;
	const unsigned int amount;

	Request(const Resource resource, const unsigned int amount) : resource(resource), amount(amount) {}
};

class Bank {
public:
	std::unordered_map<Resource, int> inventory;

	// RequestResource(request) determines if bank holds enough resources to fulfill request. If so,
	//   subtracts requested resources from bank and returns true. Otherwise returns false.
	// effects: may mutate inventory
	// time: O(1)
	bool RequestResource(Request& request);

	// RequestResource(requests) determines if bank holds enough resources to fulfill all requests. If so, 
	//   subtracts requested resources from bank and returns true. Otherwise returns false.
	// effects: may mutate inventory
	// time: O(n), where n is length of requests.
	bool RequestResources(std::vector<Request>& requests);


	Bank();

	Bank(int starting_amount);
	
	operator std::string() const;
};