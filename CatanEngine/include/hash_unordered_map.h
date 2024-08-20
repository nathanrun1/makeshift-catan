#pragma once

#include <unordered_map>
#include <functional>

template <typename T>
inline void hash_combine(std::size_t& seed, const T& val) {
	seed ^= std::hash<T>{}(val)+0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename K, typename V>
struct hash_unordered_map {
	std::size_t operator()(const std::unordered_map<K, V>& m) const {
		std::size_t seed = 0;
		for (const std::pair<K, V>& pair : m) {
			hash_combine(seed, pair.first);
			hash_combine(seed, pair.second);
		}
		return seed;
	}
};