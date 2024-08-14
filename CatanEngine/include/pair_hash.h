#pragma once

#include <utility>

namespace std {
	/// <summary>
	/// Custom std::hash defined for a std::pair. 
	/// </summary>
	template <typename T1, typename T2>
	struct hash<std::pair<T1, T2>> {
		std::size_t operator()(const std::pair<T1, T2>& p) const {
			std::size_t hash1 = std::hash<T1>{}(p.first);
			std::size_t hash2 = std::hash<T2>{}(p.second);
			return hash1 ^ (hash2 << 1);
		}
	};
}