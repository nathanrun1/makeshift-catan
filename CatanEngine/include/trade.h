#pragma once
#include <unordered_map>
#include <functional>
#include "hash_unordered_map.h"
#include "resource.h"

class Trade {
public:
	std::unordered_map<Resource, int> offer;
	std::unordered_map<Resource, int> request;

	Trade(const std::unordered_map<Resource, int> offer, const std::unordered_map<Resource, int> request);

	Trade();

	bool operator==(const Trade& other) const;

	bool operator!=(const Trade& other) const;

	operator std::string() const;

	/// <summary>
	/// Checks if the trade is valid, that is:
	/// There is at least one resource offered, and at least one resource requested.
	/// Any resource requested is not also offered, and vice versa.
	/// </summary>
	/// <returns>True if valid, false if not</returns>
	bool is_valid();
};

namespace std {
	template <>
	struct hash<Trade> {
		std::size_t operator()(const Trade& t) const {
			std::size_t seed = 0;
			hash_combine(seed, hash_unordered_map<Resource, int>{}(t.offer));
			hash_combine(seed, hash_unordered_map<Resource, int>{}(t.request));
			return seed;
		}
	};
}